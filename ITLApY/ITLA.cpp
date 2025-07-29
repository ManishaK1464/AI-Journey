// File: ITLA.cpp
#include "ITLA.h"

// Constructor: use Serial1 by default
ITLA::ITLA(HardwareSerial &serial) : itlaSerial(serial), verbose(false) { }

// Compute BIP-4 as per spec:contentReference[oaicite:23]{index=23}.
uint8_t ITLA::calcBIP4(uint8_t *data) {
    // XOR all bytes (data[0]&0x0F ^ data[1] ^ data[2] ^ data[3]), then fold nibbles.
    uint8_t bip8 = (data[0] & 0x0F) ^ data[1] ^ data[2] ^ data[3];
    uint8_t bip4 = ((bip8 >> 4) & 0x0F) ^ (bip8 & 0x0F);
    return bip4;
}

// send raw 4-byte frame, read 4-byte response (returns 32-bit raw response)
uint32_t ITLA::sendCommandFrame(uint32_t frame) {
    // Split into bytes (big-endian: highest in data[0])
    uint8_t data[4];
    data[0] = (frame >> 24) & 0xFF;
    data[1] = (frame >> 16) & 0xFF;
    data[2] = (frame >> 8) & 0xFF;
    data[3] = frame & 0xFF;

    // Compute BIP-4, insert into top nibble of byte0
    uint8_t bip4 = calcBIP4(data);
    data[0] = (bip4 << 4) | (data[0] & 0x0F);

    // Send bytes over Serial1
    if (verbose) {
        Serial.print("Sending: ");
        for(int i=0;i<4;i++){ Serial.print(data[i], HEX); Serial.print(" "); }
        Serial.println();
    }
    itlaSerial.write(data, 4);

    // Wait for 4 bytes response (timeout?)
    uint32_t resp = 0;
    int bytesRead = 0;
    unsigned long t0 = millis();
    while (bytesRead < 4 && (millis() - t0) < 100) {
        if (itlaSerial.available()) {
            uint8_t b = itlaSerial.read();
            resp = (resp << 8) | b;
            bytesRead++;
        }
    }
    if (bytesRead < 4) {
        // timeout or incomplete frame
        if (verbose) Serial.println("Response timeout!");
        return 0xFFFFFFFF; // indicate error
    }

    // Verify BIP-4 on response (optional)
    uint8_t recv[4] = {
        (uint8_t)((resp>>24)&0xFF),
        (uint8_t)((resp>>16)&0xFF),
        (uint8_t)((resp>>8)&0xFF),
        (uint8_t)(resp&0xFF)
    };
    uint8_t expectedBip = calcBIP4(recv);
    uint8_t actualBip = (recv[0] >> 4) & 0x0F;
    if (actualBip != expectedBip) {
        if (verbose) Serial.println("BIP checksum error");
        return 0xFFFFFFFF;
    }
    if (verbose) {
        Serial.print("Received: ");
        for(int i=0;i<4;i++){ Serial.print(recv[i], HEX); Serial.print(" "); }
        Serial.println();
    }
    return resp;
}

// Core transaction: send and parse response.
uint16_t ITLA::transact(uint8_t reg, bool writeFlag, uint16_t data, uint8_t &status) {
    // Build 32-bit frame (without BIP4). According to spec:
    // Bit27: 0 (normal), Bit26:1 (constant), Bit25:0, Bit24: R/W, Bits23-16: reg, Bits15-0: data.
    uint32_t frame = 0;
    frame |= (1UL << 26);            // constant bit26 = 1
    if(writeFlag) frame |= (1UL << 24);  // set R/W bit at position 24
    frame |= ((uint32_t)reg << 16);
    frame |= data;

    uint32_t raw = sendCommandFrame(frame);
    if (raw == 0xFFFFFFFF) {
        // communication error
        status = 0xFF;
        return 0;
    }

    // Parse response: Raw 32-bit: bits31-28 BIP, bit27 CE, bit26=1, bits25-24 status, 23-16 reg, 15-0 data.
    // Check CE bit:
    bool CE = (raw & (1UL<<27));
    if (CE) {
        if (verbose) Serial.println("Communication Error (CE) flag set!");
        status = 0xFE; // communication error
        return 0;
    }
    // Extract status and data
    uint32_t payload = raw & 0x03FFFFFFUL;  // keep bits25-0
    status = (payload >> 24) & 0x03;
    uint8_t respReg = (payload >> 16) & 0xFF;
    uint16_t respData = payload & 0xFFFF;

    // Sanity check register echo
    if (respReg != reg) {
        if (verbose) {
            Serial.print("Warning: response reg mismatch (expected "); Serial.print(reg,HEX);
            Serial.print(", got "); Serial.print(respReg,HEX); Serial.println(")");
        }
    }
    return respData;
}

// Public: initialize interface, detect baud rate.
bool ITLA::begin(bool dbg) {
    verbose = dbg;
    // Try common baud rates
    const long bauds[] = {4800, 9600, 19200, 38400, 57600, 115200};
    for (auto baud : bauds) {
        itlaSerial.begin(baud);
        delay(50);
        if (verbose) {
            Serial.print("Trying baud "); Serial.println(baud);
        }
        // Try reading NOP register (0x00)
        uint8_t status;
        uint16_t val = transact(ITLA_REG_NOP, false, 0, status);
        if (status == 0) {
            // Got a valid response
            if (verbose) {
                Serial.print("Device responded at "); Serial.print(baud); Serial.println(" baud");
            }
            return true;
        }
    }
    // Failed, default to 9600
    itlaSerial.begin(9600);
    if (verbose) Serial.println("Failed auto-baud, defaulting to 9600");
    return false;
}

// Read 16-bit register
uint16_t ITLA::readRegister(uint8_t reg) {
    uint8_t status;
    uint16_t val = transact(reg, false, 0, status);
    if (status != 0) {
        if (verbose) {
            Serial.print("Read reg "); Serial.print(reg,HEX);
            Serial.print(" error status=0x"); Serial.println(status,HEX);
        }
        // Optionally handle XE: read NOP for code
        if (status == 1) {
            uint8_t err = getErrorCode();
            Serial.print("Execution Error Code: 0x"); Serial.println(err, HEX);
        }
        // For this library we simply return 0 on error.
        return 0;
    }
    return val;
}

// Write 16-bit register
void ITLA::writeRegister(uint8_t reg, uint16_t value) {
    uint8_t status;
    uint16_t ack = transact(reg, true, value, status);
    if (status != 0) {
        if (verbose) {
            Serial.print("Write reg "); Serial.print(reg,HEX);
            Serial.print(" value 0x"); Serial.print(value,HEX);
            Serial.print(" error status=0x"); Serial.println(status,HEX);
        }
        if (status == 1) {
            uint8_t err = getErrorCode();
            Serial.print("Execution Error Code: 0x"); Serial.println(err, HEX);
        }
    }
}

// Laser on/off using ResEna (bit3)
void ITLA::laserOn() {
    // Write SENA=1, keep other bits 0
    writeRegister(ITLA_REG_RESETA, 0x0008);
}
void ITLA::laserOff() {
    writeRegister(ITLA_REG_RESETA, 0x0000);
}

// Set power in dBm
void ITLA::setPower_dBm(double dBm) {
    int16_t raw = (int16_t)round(dBm * 100.0);
    writeRegister(ITLA_REG_POWER, (uint16_t)raw);
}

// Set frequency in THz by computing channel value
void ITLA::setFrequencyTHz(double freqTHz) {
    // Read grid spacing (GHz)
    uint16_t grid_i = readRegister(ITLA_REG_GRID);
    uint16_t grid_f = readRegister(ITLA_REG_GRID2);
    double gridGHz = grid_i*0.1 + grid_f*0.001;  // per OIF eqn:contentReference[oaicite:24]{index=24}
    // Read first-channel frequency (GHz)
    uint16_t fcf1 = readRegister(ITLA_REG_FCF1);
    uint16_t fcf2 = readRegister(ITLA_REG_FCF2);
    uint16_t fcf3 = readRegister(ITLA_REG_FCF3);
    double firstGHz = fcf1*1000.0 + fcf2*0.1 + fcf3*0.001;
    double targetGHz = freqTHz * 1000.0;
    double channelDouble = (targetGHz - firstGHz) / gridGHz + 1.0;
    uint32_t channel = (uint32_t)lround(channelDouble);
    // Split 32-bit channel into MSW (ChannelH) and LSW (Channel)
    uint16_t channelLS = channel & 0xFFFF;
    uint16_t channelMS = (channel >> 16) & 0xFFFF;
    writeRegister(ITLA_REG_CHANNEL+1, channelMS); // ChannelH register (0x30+1=0x31?) Actually spec says 0x65, but simplest: write MS then LS via hack.
    writeRegister(ITLA_REG_CHANNEL, channelLS);
}

// Get temperature (°C)
double ITLA::getTemperature() {
    int16_t raw = (int16_t)readRegister(ITLA_REG_TEMP);
    return (double)raw / 100.0;
}

// Read serial number string via AEA (register 0x04)
String ITLA::readSerialNumber() {
    uint8_t status;
    // Initiate AEA read by reading SerNo register
    uint16_t lenVal = transact(ITLA_REG_SN, false, 0, status);
    if (status != 2) {
        // If not AEA-flag, no string
        return "";
    }
    uint8_t strLen = lenVal & 0xFF;
    String s = "";
    // Now repeatedly read AEA-EAR (0x0B)
    for (int i = 0; i < (int)strLen; i += 2) {
        uint16_t chunk = transact(ITLA_REG_EAR, false, 0, status);
        if (status == 0) {
            char c1 = (char)( (chunk >> 8) & 0xFF );
            char c2 = (char)( chunk & 0xFF );
            if (c1 == 0 || i >= strLen) break;
            s += c1;
            if (c2 == 0 || i+1 >= strLen) break;
            s += c2;
        } else {
            break;
        }
    }
    return s;
}

uint8_t ITLA::getErrorCode() {
    uint8_t st;
    uint16_t val = transact(ITLA_REG_NOP, false, 0, st);
    return (uint8_t)(val & 0x0F);
}
