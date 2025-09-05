#include <Arduino.h>
#include <EEPROM.h>
#include "ITLA.h"

ITLA itla(Serial1);

// Variables to store config
double savedFreq = 193.50;       // default THz if EEPROM empty
int32_t savedPower_milli = 0;    // power in milli-dBm for high precision
bool savedLaserEnable = false;    // always force off at startup

// EEPROM save/load
void saveConfig() {
    EEPROM.put(0, savedFreq);
    EEPROM.put(8, savedPower_milli);   // 8 bytes offset for double
    EEPROM.put(12, savedLaserEnable);
    // EEPROM.commit(); // only needed on ESP, safe on AVR
}

void loadConfig() {
    EEPROM.get(0, savedFreq);
    EEPROM.get(8, savedPower_milli);
    EEPROM.get(12, savedLaserEnable);

    // safety: always disable laser on boot
    savedLaserEnable = false;
}

// --- Periodic Sync Function --- //
void syncITLA() {
    // Read actual laser state
    double currentFreq = itla.getFrequencyTHz();
    double currentPower = itla.getPower_dBm();
    double tempC = itla.getTemperature();

    // Send JSON to GUI
    Serial.print("{\"freq\":");
    Serial.print(currentFreq, 6);    // 6 decimals for THz
    Serial.print(",\"power\":");
    Serial.print(currentPower, 3);   // 3 decimals for dBm
    Serial.print(",\"temp\":");
    Serial.print(tempC, 2);          // 2 decimals for °C
    Serial.println("}");
}

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("ITLA Test Start");

    if (!itla.begin(true)) {
        Serial.println("ITLA not responding!");
        while (1);
    }
    Serial.println("ITLA connected.");

    // Load saved config
    loadConfig();
    Serial.println("Loaded last configuration from EEPROM");

    // Apply power & frequency, but keep laser OFF for safety
    itla.setPower_dBm(savedPower_milli / 1000.0);  // convert milli-dBm to dBm
    itla.setFrequencyTHz(savedFreq);
    itla.laserOff();
    Serial.println("Laser forced OFF at startup for safety.");
}

void loop() {
    // --- 1. Handle GUI / Serial Commands --- //
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        processCommand(command);
    }

    // --- 2. Periodic Sync --- //
    static unsigned long lastSync = 0;
    if (millis() - lastSync > 200) {  // every 200 ms
        syncITLA();
        lastSync = millis();
    }
}

void processCommand(const String &cmd) {
    if (cmd == "LASER_ON") {
        itla.laserOn();
        savedLaserEnable = true;
        saveConfig();
        Serial.println("Laser turned ON (saved to EEPROM)");

    } else if (cmd == "LASER_OFF") {
        itla.laserOff();
        savedLaserEnable = false;
        saveConfig();
        Serial.println("Laser turned OFF (saved to EEPROM)");

    } else if (cmd.startsWith("SET_POWER")) {
        double power = cmd.substring(10).toDouble(); // dBm
        itla.setPower_dBm(power);
        savedPower_milli = (int32_t)(power * 1000);  // store in milli-dBm
        saveConfig();
        Serial.print("Power set to ");
        Serial.print(power, 3);
        Serial.println(" dBm (saved to EEPROM)");

    } else if (cmd.startsWith("SET_FREQUENCY")) {
        double frequency = cmd.substring(14).toDouble();
        itla.setFrequencyTHz(frequency);
        savedFreq = frequency;
        saveConfig();
        Serial.print("Frequency set to ");
        Serial.print(frequency, 6);
        Serial.println(" THz (saved to EEPROM)");

    } else if (cmd == "GET_TEMPERATURE") {
        double tempC = itla.getTemperature();
        Serial.print("Temperature: ");
        Serial.print(tempC, 2);
        Serial.println(" °C");

    } else if (cmd == "GET_FREQUENCY") {
        double freq = itla.getFrequencyTHz();
        Serial.print("Frequency: ");
        Serial.print(freq, 6);
        Serial.println(" THz");

    } else if (cmd == "GET_POWER") {
        double power = itla.getPower_dBm();
        Serial.print("Power: ");
        Serial.print(power, 3);
        Serial.println(" dBm");

    } else if (cmd == "GET_MANUFACTURER") {
        String manuf = itla.readAEAString(ITLA_REG_MANUF);
        Serial.print("Manufacturer: ");
        Serial.println(manuf);

    } else {
        Serial.println("Unknown command");
    }
}
