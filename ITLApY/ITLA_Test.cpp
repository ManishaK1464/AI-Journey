// File: ITLA_Test.ino
#include <Arduino.h>
#include "ITLA.h"

ITLA itla(Serial1);

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
    itla.laserOn();
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

    Serial.println("ITLA test complete.");
}

void loop() {
    // Could poll status or do other tasks here...
}
