// File: ITLA.h
#ifndef ITLA_H
#define ITLA_H

#include <Arduino.h>
#include "ITLA_Registers.h"

class ITLA {
public:
    // Constructor: allow passing the HardwareSerial (default Serial1 on Due)
    ITLA(HardwareSerial &serial = Serial1); // &serial is a reference to avoid copying // ITLA(int x) its a constructor that initializes the ITLA object with a reference to a HardwareSerial object.

    // Initialize with optional debug flag. Returns true if module found.
    bool begin(bool verbose = false);

    // Read/write 16-bit register (returns data or throws on error)
    uint16_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint16_t value);

    // Laser control
    void laserOn();      // Turn laser output on (sets SENA bit)
    void laserOff();     // Turn laser output off (clears SENA)

    // Set/get optical power (dBm)
    void setPower_dBm(double dBm);

    // Set wavelength by frequency (THz)
    void setFrequencyTHz(double freqTHz);

    // Get temperature (°C)
    double getTemperature();

    // Read serial number (as string via AEA)
    //String readSerialNumber();

    // Read string from AEA register (e.g., SN)
    String readAEAString(uint8_t reg);

    // Verbose debug output on Serial (USB)
    void setVerbose(bool on);

private:
    HardwareSerial &itlaSerial; // Reference to the serial port a permanent reference to avoid copying
    bool verbose;

    // Send a raw command frame; returns raw 32-bit response.
    uint32_t sendCommandFrame(uint32_t frame);
    // Build and send a command, then parse response.
    // writeFlag=1 for write, 0 for read. reg=8-bit, data=16-bit.
    // Returns the 16-bit data field of response; status out by reference.
    uint16_t transact(uint8_t reg, bool writeFlag, uint16_t data, uint8_t &status);

    // Compute BIP-4 checksum (for 4-byte array).
    uint8_t calcBIP4(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

    void formCommandPacket(uint8_t* outData,
                       uint8_t command,
                       uint16_t value,
                       uint8_t isWrite,
                       uint8_t lstRsp);

    // Read NOP register to get error field (bits 3:0).
    uint8_t getErrorCode();

    // Delay or check SRQ if needed for pending ops (not implemented here).
    // ...
};

#endif // ITLA_H
