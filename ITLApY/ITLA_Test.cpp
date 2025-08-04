// File: ITLA_Test.ino
#include <Arduino.h>
#include "ITLA.h"

ITLA itla(Serial1); // object for ITLA communication ITLA class itla object with Serial1 itla.any function define in ITLA.h
// serial1 passing to ITLA class constructor as a parameter
// when someone create an object of ITLA class, it will use Serial1 by default
// inside ITLA class constructor, itlaSerial will be initialized with Serial1 serial to serial1

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("ITLA Test Start");

    bool ok = itla.begin(true); // verbose debug
    if (!ok) {
        Serial.println("ITLA not responding!");
        while(1);
    }
    Serial.println("ITLA connected.");

    // Turn laser ON
  /*/  itla.laserOn();
    delay(500);

    // Set power to 10 dBm
    itla.setPower_dBm(10.0);
    Serial.println("Set power to 10 dBm.");

    // Set wavelength to 1550.12 nm (~193.4 THz)
    itla.setFrequencyTHz(193.40);
    Serial.println("Set frequency to 193.40 THz.");

    // Read temperature
    double tempC = itla.getTemperature();
    Serial.print("Temperature: "); Serial.print(tempC); Serial.println(" °C");

    // Read serial number
    String sn = itla.readSerialNumber();
    Serial.print("Serial Number: "); Serial.println(sn);

    */
    // Reading mfgr not a optical setting so no need to turn laser on  SENA bit

    String manuf = itla.readAEARegister(ITLA_REG_MANUF);
    Serial.print("Manufacturer Register (0x02): 0x");
    Serial.println(manuf, HEX);

    Serial.println("ITLA test complete.");
}

void loop() {
    .
}
