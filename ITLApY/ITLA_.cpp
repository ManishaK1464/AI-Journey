#include "ITLA.h"
// Constructor: use Serial1 by default
ITLA::ITLA(HardwareSerial &serial) : itlaSerial(serial), verbose(false)
{
    
}

// Calculate BIP-4 checksum using lower nibble of data[0] and XOR logic
uint8_t ITLA::calcBIP4(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) {
    uint8_t bip8 = (d0 & 0x0F) ^ d1 ^ d2 ^ d3;
    return ((bip8 & 0xF0) >> 4) ^ (bip8 & 0x0F);
}

// Form a 4-byte command packet with proper header, command, value, and BIP
void ITLA::formCommandPacket(uint8_t* outData, uint8_t command, uint16_t value, uint8_t isWrite, uint8_t lstRsp) {
    outData[0] = (lstRsp ? 0x80 : 0x00) | (isWrite ? 0x08 : 0x00);
    outData[1] = command;
    outData[2] = (value >> 8) & 0xFF;
    outData[3] = value & 0xFF;

    uint8_t bip4 = calcBIP4(outData[0], outData[1], outData[2], outData[3]);
    outData[0] = (outData[0] & 0x0F) | (bip4 << 4);
}

// Send 4-byte command frame and receive 4-byte response
uint32_t ITLA::sendCommandFrame(uint32_t frame) {
    uint8_t command = (frame >> 16) & 0xFF;
    uint16_t value = frame & 0xFFFF;
    uint8_t writeFlag = (frame & (1UL << 24)) ? 1 : 0;

    uint8_t data[4];
    formCommandPacket(data, command, value, writeFlag, 0); // lstRsp = 0

    /*if (verbose) {
        Serial.print("Sending: ");
        for (int i = 0; i < 4; i++) {
            Serial.print(data[i], HEX); Serial.print(" ");
        }
        Serial.println();
    }*/  //optionally print the command frame being sent

    itlaSerial.write(data, 4);

    // Wait for 4-byte response
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
        if (verbose) Serial.println("Response timeout!");
        return 0xFFFFFFFF;
    }

    // Check BIP on response
    uint8_t recv[4] = {
        (uint8_t)((resp >> 24) & 0xFF),
        (uint8_t)((resp >> 16) & 0xFF),
        (uint8_t)((resp >> 8) & 0xFF),
        (uint8_t)(resp & 0xFF)
    };
    uint8_t expectedBip = calcBIP4(recv[0] & 0x0F, recv[1], recv[2], recv[3]);
    uint8_t actualBip = (recv[0] >> 4) & 0x0F;
    if (actualBip != expectedBip) {
        if (verbose) Serial.println("BIP checksum error");
        return 0xFFFFFFFF;
    }

    if (verbose) {
        Serial.print("Received: ");
        for (int i = 0; i < 4; i++) {
            Serial.print(recv[i], HEX); Serial.print(" ");
        }
        Serial.println();
    }

    return resp;
}

// Perform transaction: send command, get parsed data + status wrapper for send and receive
uint16_t ITLA::transact(uint8_t reg, bool writeFlag, uint16_t data, uint8_t &status) {
    uint32_t frame = 0;
    frame |= (1UL << 26);                    // fixed bit 26 to 1 ,1 unsigned long  32 bit 00000100000000000000000000000000 0x04000000
    if (writeFlag) frame |= (1UL << 24);     // R/W bit 24 set to 1 for write, 0 for read
    frame |= ((uint32_t)reg << 16);          // register  0x12 << 16 = 0x00120000
    frame |= data;                           // data

    uint32_t raw = sendCommandFrame(frame);
    if (raw == 0xFFFFFFFF) {
        status = 0xFF;
        return 0;
    }

    bool CE = (raw & (1UL << 27));
    if (CE) {
        if (verbose) Serial.println("Communication Error (CE) flag set!");
        status = 0xFE;
        return 0;
    }

    uint32_t payload = raw & 0x03FFFFFFUL;
    status = (payload >> 24) & 0x03;  // status bits 25:24
    uint8_t respReg = (payload >> 16) & 0xFF;
    uint16_t respData = payload & 0xFFFF;

    if (respReg != reg && verbose) {
        Serial.print("Warning: response reg mismatch (expected ");
        Serial.print(reg, HEX);
        Serial.print(", got ");
        Serial.print(respReg, HEX);
        Serial.println(")");
    }

    return respData;
}


//verbose mode is used to print debug information dbg = true enables verbose mode
bool ITLA::begin(bool dbg) {
    verbose = dbg;
    const long bauds[] = {4800, 9600, 19200, 38400, 57600, 115200};
    const size_t nBauds = sizeof(bauds) / sizeof(bauds[0]);

    for (size_t i = 0; i < nBauds; ++i) {
        long baud = bauds[i];
        itlaSerial.begin(baud);
        delay(50);
        if (verbose) {
            Serial.print("Trying baud ");
            Serial.println(baud);
        }

        uint8_t status;
        uint16_t dummy = transact(ITLA_REG_NOP, false, 0, status);
        // Check status, not the returned value
        if (status == 0) {
            if (verbose) {
                Serial.print("Device responded at ");
                Serial.print(baud);
                Serial.println(" baud");
            }
            return true;
        }
    }

    // None responded—default back to 9600
    itlaSerial.begin(9600);
    if (verbose) {
        Serial.println("Failed auto-baud, defaulting to 9600");
    }
    return false;
}


// additional check for baud rate detection in pdf its 9600 given as default 
/*bool ITLA::begin(bool dbg) {
    verbose = dbg;
    const long bauds[] = {4800, 9600, 19200, 38400, 57600, 115200};
    for (auto baud : bauds) {
        itlaSerial.begin(baud);
        delay(50);
        if (verbose) {
            Serial.print("Trying baud "); Serial.println(baud);
        }
        uint8_t status;
        uint16_t val = transact(ITLA_REG_NOP, false, 0, status);
        if (status == 0) {
            if (verbose) {
                Serial.print("Device responded at "); Serial.print(baud); Serial.println(" baud");
            }
            return true;
        }
    }
    itlaSerial.begin(9600);
    if (verbose) Serial.println("Failed auto-baud, defaulting to 9600");
    return false;
} */

uint16_t ITLA::readRegister(uint8_t reg) {
    uint8_t status;
    uint16_t val = transact(reg, false, 0, status);
    if (status != 0) {
        if (verbose) {
            Serial.print("Read reg "); Serial.print(reg, HEX);
            Serial.print(" error status=0x"); Serial.println(status, HEX);
        }
        if (status == 1) {
            uint8_t err = getErrorCode();
            Serial.print("Execution Error Code: 0x"); Serial.println(err, HEX);
        }
        return 0;
    }
    return val;
}

void ITLA::writeRegister(uint8_t reg, uint16_t value) {
    uint8_t status;
    uint16_t ack = transact(reg, true, value, status);
    if (status != 0) {
        if (verbose) {
            Serial.print("Write reg "); Serial.print(reg, HEX);
            Serial.print(" value 0x"); Serial.print(value, HEX);
            Serial.print(" error status=0x"); Serial.println(status, HEX);
        }
        if (status == 1) {
            uint8_t err = getErrorCode();
            Serial.print("Execution Error Code: 0x"); Serial.println(err, HEX);
        }
    }
}

void ITLA::laserOn() {
    writeRegister(ITLA_REG_RESETA, 0x0008);
}

void ITLA::laserOff() {
    writeRegister(ITLA_REG_RESETA, 0x0000);
}

void ITLA::setPower_dBm(double dBm) {
    int16_t raw = (int16_t)round(dBm * 100.0);
    writeRegister(ITLA_REG_POWER, (uint16_t)raw);
}

void ITLA::setFrequencyTHz(double freqTHz) {
    // Read grid spacing
    uint16_t grid_i = readRegister(ITLA_REG_GRID);
    uint16_t grid_f = readRegister(ITLA_REG_GRID2);
    double gridGHz = grid_i * 0.1 + grid_f * 0.001;

    // Read first channel frequency
    uint16_t fcf1 = readRegister(ITLA_REG_FCF1);
    uint16_t fcf2 = readRegister(ITLA_REG_FCF2);
    uint16_t fcf3 = readRegister(ITLA_REG_FCF3);
    double firstGHz = fcf1 * 1000.0 + fcf2 * 0.1 + fcf3 * 0.001;

    // Compute channel number
    double targetGHz = freqTHz * 1000.0;
    double channelDouble = (targetGHz - firstGHz) / gridGHz + 1.0;
    uint32_t channel = (uint32_t)lround(channelDouble);

    // Split into high/low 16 bits
    uint16_t channelLS = channel & 0xFFFF;
    uint16_t channelMS = (channel >> 16) & 0xFFFF;

    // Write high word first, then low word
    writeRegister(ITLA_REG_CHANNELH, channelMS);  // 0x65
    writeRegister(ITLA_REG_CHANNEL, channelLS);   // 0x30
}

double ITLA::getFrequencyLF() {
    uint16_t lf1 = readRegister(ITLA_REG_LF1); // THz
    uint16_t lf2 = readRegister(ITLA_REG_LF2); // GHz*10
    uint16_t lf3 = readRegister(ITLA_REG_LF3); // MHz

    double freqGHz = lf1 * 1000.0
                   + lf2 * 0.1
                   + lf3 * 0.001;

    return freqGHz / 1000.0; // THz
}


double ITLA::getTemperature() {
    int16_t raw = (int16_t)readRegister(ITLA_REG_TEMP);
    return (double)raw / 100.0;
}

/*String ITLA::readSerialNumber() {
    uint8_t status;
    uint16_t lenVal = transact(ITLA_REG_SN, false, 0, status);
    if (status != 2) return "";
    uint8_t strLen = lenVal & 0xFF;
    String s = "";
    for (int i = 0; i < (int)strLen; i += 2) {
        uint16_t chunk = transact(ITLA_REG_EAR, false, 0, status);
        if (status == 0) {
            char c1 = (char)((chunk >> 8) & 0xFF);
            char c2 = (char)(chunk & 0xFF);
            if (c1 == 0 || i >= strLen) break;
            s += c1;
            if (c2 == 0 || i + 1 >= strLen) break;
            s += c2;
        } else break;
    }
    return s; */
/*String ITLA::readAEAString(uint8_t reg) {
    uint8_t status;
    uint16_t lenVal = transact(reg,false,0,status);
    if (status != 2) return "";
    uint8_t strLen = lenVal & 0xFF;
    String s;
    for (int i = 0; i < strLen; i += 2) {
        uint16_t chunk = transact(ITLA_REG_EAR,false,0,status);
        if (status != 0) break;
        char c1 = (chunk>>8)&0xFF;
        char c2 = chunk&0xFF;
        if (c1==0 || i >= strLen) break;
        s += c1;
        if (c2==0 || i+1>=strLen) break;
        s += c2;
    }
    return s;
}*/
String ITLA::readAEAString(uint8_t reg) {
    uint8_t st;
    // 1) fetch length (status==2)
    uint16_t lenVal = transact(reg, false, 0, st);
    if (st != 2) {
        if (verbose) {
            Serial.print("AEA length read error, status=0x");
            Serial.println(st, HEX);
        }
        return "";
    }
    uint8_t strLen = lenVal & 0xFF;

    // 2) read in 2-byte chunks
    String s;
    for (int i = 0; i < strLen; i += 2) {
        uint16_t chunk = transact(ITLA_REG_EAR, false, 0, st);
        if (st != 0) {
            if (verbose) {
                Serial.print("AEA chunk read error at byte ");
                Serial.print(i);
                Serial.print(", status=0x");
                Serial.println(st, HEX);
            }
            break;
        }
        char c1 = (chunk >> 8) & 0xFF;
        char c2 =  chunk       & 0xFF;
        if (c1 == '\0') break;
        s += c1;
        if (i + 1 >= strLen || c2 == '\0') break;
        s += c2;
    }
    return s;
}

uint8_t ITLA::getErrorCode() {
    uint8_t st;
    uint16_t val = transact(ITLA_REG_NOP, false, 0, st);
    return (uint8_t)(val & 0x0F);
}
// Set verbose mode
void ITLA::setVerbose(bool on) {
    verbose = on;
}


// need to confirm 
double ITLA::getPower_dBm() {
    int16_t raw = (int16_t)readRegister(ITLA_REG_POWER);
    return raw / 100.0;
}

double ITLA::getFrequencyTHz() {
    uint16_t channelLS = readRegister(ITLA_REG_CHANNEL);       // 0x30
    uint16_t channelMS = readRegister(ITLA_REG_CHANNELH);      // 0x65
    uint32_t channel = ((uint32_t)channelMS << 16) | channelLS;

    uint16_t grid_i = readRegister(ITLA_REG_GRID);
    uint16_t grid_f = readRegister(ITLA_REG_GRID2);
    double gridGHz = grid_i * 0.1 + grid_f * 0.001;

    uint16_t fcf1 = readRegister(ITLA_REG_FCF1);
    uint16_t fcf2 = readRegister(ITLA_REG_FCF2);
    uint16_t fcf3 = readRegister(ITLA_REG_FCF3);
    double firstGHz = fcf1*1000.0 + fcf2*0.1 + fcf3*0.001;

    double freqGHz = firstGHz + (channel - 1) * gridGHz;
    return freqGHz / 1000.0; // THz
}

bool ITLA::isLaserOn() {
    uint16_t val = readRegister(ITLA_REG_RESETA);
    return (val & 0x08) != 0;
}
