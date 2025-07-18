struct ITLAPacket {
    uint8_t data[4];
};

//data processing printing and simulation

uint8_t calculatBIP4 (unit8_t data0 , uint8_t data1, uint8_t data2, uint8_t data3) {
  uint8_t bip8 = (data0 & 0x0F) ^ data1 ^ data2 ^ data3;
  // masking the data0 to only use the lower 4 bits
    return ((bip8 & 0xF0) >> 4) ^ (bip8 & 0x0F);

}

void formCommandPacket(struct ITLAPacket* packet, uint8_t command, uint16_t value, uint8_t isWrite, uint8_t lstRsp) {
    // ITLAPacket* packet  pointer to an ITLAPacket structure where the packet data will be stored 

        packet->data[0] = (lstRsp ? 0x80 : 0x00) | (isWrite ? 0x08 : 0x00);
        packet->data[1] = command;
        packet->data[2] = (value >> 8) & 0xFF; // stores upper 8 bits of value
        packet->data[3] = value & 0xFF; // stores lower 8 bits of value

    uint8_t bip4 = calculateBIP4(packet->data[0], packet->data[1], packet->data[2], packet->data[3]);
      packet->data[0] = (packet->data[0] & 0x0F) | (bip4 << 4);

      // inserts upper 4 bits of BIP4 into the upper 4 bits of data[0] as checksum 
      // packet->data[0] preserves only lower 4 bits and inserts BIP4 in upper 4 bits 

  uint32_t fullChannel = ((uint32_t)0x000F << 16) | 0x00AA; 

  // ---------- Simulate Sending Packet ---------- heler or abstraction layer 
void sendPacket(bool isWrite, uint8_t reg, uint16_t value, const char* label, bool lstRsp = true) {
  ITLAPacket packet;
  formCommandPacket(&packet, reg, value, isWrite, lstRsp);
  printPacket(&packet, label);
}
