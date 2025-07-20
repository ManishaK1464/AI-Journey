#include <Arduino.h>

// ---------- ITLA Packet Structure ----------
struct ITLAPacket {
  uint8_t data[4];
};

// ---------- BIP-4 Checksum ----------
uint8_t calculateBIP4(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3) {
  uint8_t bip8 = (data0 & 0x0F) ^ data1 ^ data2 ^ data3;
  return ((bip8 & 0xF0) >> 4) ^ (bip8 & 0x0F);
}

// Last valid response storage
ITLAPacket lastResponse;

// ---------- Form ITLA Packet ----------
void formCommandPacket(ITLAPacket* packet,
                       uint8_t command,
                       uint16_t value,
                       bool isWrite,
                       bool lstRsp) {
  // Flags in byte0: bit7 = lstRsp, bit3 = isWrite
  packet->data[0] = (lstRsp ? 0x80 : 0x00) | (isWrite ? 0x08 : 0x00);
  packet->data[1] = command;
  packet->data[2] = (value >> 8) & 0xFF;
  packet->data[3] = value & 0xFF;

  // Compute and insert checksum in upper nibble of byte0
  uint8_t bip4 = calculateBIP4(packet->data[0], packet->data[1], packet->data[2], packet->data[3]);
  packet->data[0] = (packet->data[0] & 0x0F) | (bip4 << 4);
}

// ---------- Send Packet ----------
bool sendPacket(bool isWrite,
                uint8_t reg,
                uint16_t value,
                const char* label = nullptr,
                bool lstRsp = true) {
  ITLAPacket packet;
  formCommandPacket(&packet, reg, value, isWrite, lstRsp);

  if (label) {
    Serial.print("→ ");
    Serial.println(label);
  }

  // Transmit 4 bytes
  for (int i = 0; i < 4; i++) {
    Serial.write(packet.data[i]);
    // Also print them for debug
    Serial.print("0x");
    if (packet.data[i] < 0x10) Serial.print('0');
    Serial.print(packet.data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();

  // Receive if read or last response expected
  if (!isWrite || lstRsp) {
    unsigned long timeout = millis() + 500;
    int idx = 0;
    ITLAPacket resp;
    while (idx < 4 && millis() < timeout) {
      if (Serial.available()) {
        resp.data[idx++] = Serial.read();
      }
    }
    if (idx < 4) {
      Serial.println("⨯ Timeout waiting for response");
      return false;
    }
    uint8_t rc = (resp.data[0] & 0xF0) >> 4;
    uint8_t cc = calculateBIP4(resp.data[0], resp.data[1], resp.data[2], resp.data[3]);
    if (rc != cc) {
      Serial.print("⨯ Checksum mismatch: Rcv=");
      Serial.print(rc, HEX);
      Serial.print(" Calc=");
      Serial.println(cc, HEX);
      return false;
    }
    lastResponse = resp;
    // Print human-readable
    uint8_t regR = resp.data[1];
    uint16_t valR = ((uint16_t)resp.data[2] << 8) | resp.data[3];
    Serial.print("✓ Response R0x"); Serial.print(regR, HEX);
    Serial.print(": 0x"); Serial.println(valR, HEX);
    return true;
  }
  return true;
}

// ---------- Read AEA String (e.g., MFGR, Model, Serial) ----------
String readAEAString(uint8_t reg) {
  // 1) Request string length via AEA error
  sendPacket(false, reg, 0, nullptr, true);
  uint16_t byteCount = ((uint16_t)lastResponse.data[2] << 8) | lastResponse.data[3];
  String result = "";
  uint16_t remaining = byteCount;

  // 2) Read 2 bytes at a time from AEA-EAR (0x0B)
  while (remaining > 0) {
    sendPacket(false, 0x0B, 0, nullptr, true);
    result += char(lastResponse.data[2]);
    if (remaining > 1) result += char(lastResponse.data[3]);
    remaining -= 2;
  }
  return result;
}

// ---------- Write Full 32-bit Channel with Polling ----------
bool writeChannel32(uint32_t freq32) {
  uint16_t high = (freq32 >> 16) & 0xFFFF;
  uint16_t low  = freq32 & 0xFFFF;

  // Write high and low parts
  sendPacket(true, 0x65, high, "Write ChannelH", true);
  sendPacket(true, 0x30, low,  "Write Channel",  true);

  // Poll NOP (0x00) until pending flag clears
  while (true) {
    sendPacket(false, 0x00, 0, nullptr, true);
    uint8_t status = lastResponse.data[0] & 0x03;    // error bits 0-1
    bool pending  = lastResponse.data[0] & 0x10;    // CP flag (bit4)
    if (!pending) {
      if (status != 0x00) {
        Serial.println("⨯ Tuning error");
        return false;
      }
      return true;
    }
    delay(50);
  }
}

// ---------- Arduino Setup & Loop ----------
void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("=== ITLA Full Sketch ===");

  // Example: Read Manufacturer String
  String mfgr = readAEAString(0x02);
  Serial.println("MFGR: " + mfgr);

  // Example: Tune to channel 0x000F00AA
  uint32_t freq = 0x000F00AA;
  if (writeChannel32(freq)) {
    Serial.print("Channel set to 0x");
    Serial.println(freq, HEX);
  }
}

void loop() {
  // No repeating actions
}
