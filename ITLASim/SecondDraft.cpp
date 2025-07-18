struct ITLAPacket {
  uint8_t data[4];
};

// ---------- Calculate BIP-4 Checksum ----------
uint8_t calculateBIP4(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3) {
  uint8_t bip8 = (data0 & 0x0F) ^ data1 ^ data2 ^ data3;
  return ((bip8 & 0xF0) >> 4) ^ (bip8 & 0x0F);
}

// ---------- Form ITLA Packet ----------
void formCommandPacket(struct ITLAPacket* packet, uint8_t command, uint16_t value, uint8_t isWrite, uint8_t lstRsp) {
  packet->data[0] = (lstRsp ? 0x80 : 0x00) | (isWrite ? 0x08 : 0x00);
  packet->data[1] = command;
  packet->data[2] = (value >> 8) & 0xFF;
  packet->data[3] = value & 0xFF;
  uint8_t bip4 = calculateBIP4(packet->data[0], packet->data[1], packet->data[2], packet->data[3]);
  packet->data[0] = (packet->data[0] & 0x0F) | (bip4 << 4);
}

// ---------- Send Packet over UART ----------
void sendPacket(bool isWrite, uint8_t reg, uint16_t value, const char* label = nullptr, bool lstRsp = true) {
  ITLAPacket packet;
  formCommandPacket(&packet, reg, value, isWrite, lstRsp);

  if (label) {
    Serial.print("→ ");
    Serial.println(label);
  }

  Serial.print("Sending: ");
  for (int i = 0; i < 4; i++) {
    Serial.print("0x");
    if (packet.data[i] < 0x10) Serial.print("0");
    Serial.print(packet.data[i], HEX);
    Serial.print(" ");
    Serial.write(packet.data[i]);  // Transmit byte to ITLA device
  }
  Serial.println();
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("=== ITLA Packet Transmission ===");

  sendPacket(false, 0x02, 0x0000, "Reading Register 0x02 (status)");
  sendPacket(true,  0x65, 0x000F, "ChannelH (upper 16-bit)");
  sendPacket(true,  0x30, 0x00AA, "Channel (lower 16-bit, commit)");

  uint32_t fullChannel = ((uint32_t)0x000F << 16) | 0x00AA;
  Serial.print("→ Full 32-bit Channel Value: 0x");
  Serial.println(fullChannel, HEX);
}

// ---------- Loop ----------
void loop() {}
