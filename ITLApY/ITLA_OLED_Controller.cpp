// File: ITLA_OLED_Controller.cpp
// Integrated ITLA Control with OLED Display and Button Interface

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ITLA.h"

// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // creating display object class obj(ect)
//  obj.begin() is method calling on obj
// blueprint object and methods tells object to do something

// Button pins
#define BUTTON_UP_PIN 2
#define BUTTON_DOWN_PIN 3
#define BUTTON_INC_PIN 4
#define BUTTON_DEC_PIN 5
#define BUTTON_OK_PIN 6

// ITLA instance
ITLA itla(Serial1);

// Menu system enums
enum MenuState {
  MAIN_MENU,
  DEVICE_INFO,
  LASER_CONTROL,
  POWER_SETTINGS,
  FREQUENCY_SETTINGS,
  TEMPERATURE_MONITOR,
  STATUS_MONITOR,
  ADVANCED_SETTINGS
};

enum LaserControlState {
  LASER_OFF,
  LASER_ON,
  LASER_UNKNOWN
};

// Global variables
MenuState currentMenu = MAIN_MENU;
int menuIndex = 0;
bool deviceConnected = false;
LaserControlState laserState = LASER_UNKNOWN;
double currentPower = 0.0;
double currentFreq = 0.0;
double currentTemp = 0.0;

// Button debouncing
unsigned long lastButtonPress = 0;
const unsigned long BUTTON_DEBOUNCE = 200;

// Menu items for main menu
const char* mainMenuItems[] = {
  "Device Info",
  "Laser Control", 
  "Power Settings",
  "Freq Settings",
  "Temperature",
  "Status Monitor",
  "Advanced"
};
const int mainMenuCount = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]);

// Power settings
double powerSetpoint = 0.0;
const double POWER_MIN = -20.0;
const double POWER_MAX = 20.0;
const double POWER_STEP = 0.1;

// Frequency settings
double freqSetpoint = 193.1; // THz
const double FREQ_MIN = 191.0;
const double FREQ_MAX = 196.0;
const double FREQ_STEP = 0.01;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("ITLA OLED Controller Starting...");
  
  // Initialize buttons
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_INC_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DEC_PIN, INPUT_PULLUP);
  pinMode(BUTTON_OK_PIN, INPUT_PULLUP);
  
  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Initializing ITLA..."));
  display.display();
  
  // Initialize ITLA with verbose output
  deviceConnected = itla.begin(true);
  
  if (deviceConnected) {
    Serial.println("ITLA connected successfully!");
    // Read initial values
    updateCurrentValues();
  } else {
    Serial.println("ITLA connection failed!");
  }
  
  displayCurrentMenu();
}

void loop() {
  handleButtons();
  
  // Update values periodically when monitoring
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    if (deviceConnected && (currentMenu == TEMPERATURE_MONITOR || currentMenu == STATUS_MONITOR)) {
      updateCurrentValues();
      displayCurrentMenu();
    }
    lastUpdate = millis();
  }
}

void handleButtons() {
  if (millis() - lastButtonPress < BUTTON_DEBOUNCE) return;
  
  if (digitalRead(BUTTON_UP_PIN) == LOW) {
    handleUpButton();
    lastButtonPress = millis();
  }
  else if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
    handleDownButton();
    lastButtonPress = millis();
  }
  else if (digitalRead(BUTTON_INC_PIN) == LOW) {
    handleIncButton();
    lastButtonPress = millis();
  }
  else if (digitalRead(BUTTON_DEC_PIN) == LOW) {
    handleDecButton();
    lastButtonPress = millis();
  }
  else if (digitalRead(BUTTON_OK_PIN) == LOW) {
    handleOkButton();
    lastButtonPress = millis();
  }
}

void handleUpButton() {
  switch (currentMenu) {
    case MAIN_MENU:
      menuIndex = (menuIndex - 1 + mainMenuCount) % mainMenuCount;
      displayCurrentMenu();
      break;
    default:
      // Go back to main menu
      currentMenu = MAIN_MENU;
      menuIndex = 0;
      displayCurrentMenu();
      break;
  }
}

void handleDownButton() {
  switch (currentMenu) {
    case MAIN_MENU:
      menuIndex = (menuIndex + 1) % mainMenuCount;
      displayCurrentMenu();
      break;
    default:
      // Navigation within submenus can be added here
      break;
  }
}

void handleIncButton() {
  switch (currentMenu) {
    case POWER_SETTINGS:
      if (powerSetpoint < POWER_MAX) {
        powerSetpoint += POWER_STEP;
        displayCurrentMenu();
      }
      break;
    case FREQUENCY_SETTINGS:
      if (freqSetpoint < FREQ_MAX) {
        freqSetpoint += FREQ_STEP;
        displayCurrentMenu();
      }
      break;
  }
}

void handleDecButton() {
  switch (currentMenu) {
    case POWER_SETTINGS:
      if (powerSetpoint > POWER_MIN) {
        powerSetpoint -= POWER_STEP;
        displayCurrentMenu();
      }
      break;
    case FREQUENCY_SETTINGS:
      if (freqSetpoint > FREQ_MIN) {
        freqSetpoint -= FREQ_STEP;
        displayCurrentMenu();
      }
      break;
  }
}

void handleOkButton() {
  switch (currentMenu) {
    case MAIN_MENU:
      // Enter selected submenu
      switch (menuIndex) {
        case 0: currentMenu = DEVICE_INFO; break;
        case 1: currentMenu = LASER_CONTROL; break;
        case 2: currentMenu = POWER_SETTINGS; break;
        case 3: currentMenu = FREQUENCY_SETTINGS; break;
        case 4: currentMenu = TEMPERATURE_MONITOR; break;
        case 5: currentMenu = STATUS_MONITOR; break;
        case 6: currentMenu = ADVANCED_SETTINGS; break;
      }
      displayCurrentMenu();
      break;
      
    case LASER_CONTROL:
      toggleLaser();
      break;
      
    case POWER_SETTINGS:
      if (deviceConnected) {
        itla.setPower_dBm(powerSetpoint);
        displayConfirmation("Power Set!");
        updateCurrentValues();
      }
      break;
      
    case FREQUENCY_SETTINGS:
      if (deviceConnected) {
        itla.setFrequencyTHz(freqSetpoint);
        displayConfirmation("Frequency Set!");
        updateCurrentValues();
      }
      break;
  }
}

void toggleLaser() {
  if (!deviceConnected) return;
  
  if (laserState == LASER_OFF) {
    itla.laserOn();
    laserState = LASER_ON;
    displayConfirmation("Laser ON!");
    Serial.println("Laser turned ON");
  } else if (laserState == LASER_ON) {
    itla.laserOff();
    laserState = LASER_OFF;
    displayConfirmation("Laser OFF!");
    Serial.println("Laser turned OFF");
  }
  
  delay(1000);
  displayCurrentMenu();
}

void updateCurrentValues() {
  if (!deviceConnected) return;
  
  currentTemp = itla.getTemperature();
  // Add more value updates as needed
}

void displayCurrentMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  switch (currentMenu) {
    case MAIN_MENU:
      displayMainMenu();
      break;
    case DEVICE_INFO:
      displayDeviceInfo();
      break;
    case LASER_CONTROL:
      displayLaserControl();
      break;
    case POWER_SETTINGS:
      displayPowerSettings();
      break;
    case FREQUENCY_SETTINGS:
      displayFrequencySettings();
      break;
    case TEMPERATURE_MONITOR:
      displayTemperatureMonitor();
      break;
    case STATUS_MONITOR:
      displayStatusMonitor();
      break;
    case ADVANCED_SETTINGS:
      displayAdvancedSettings();
      break;
  }
  
  display.display();
}

void displayMainMenu() {
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println(F("ITLA Controller"));
  display.println(deviceConnected ? F("Status: Connected") : F("Status: Disconnected"));
  display.println();
  
  for (int i = 0; i < mainMenuCount; i++) {
    if (i == menuIndex) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }
    display.print(F("> "));
    display.println(mainMenuItems[i]);
  }
}

void displayDeviceInfo() {
  display.setCursor(0, 0);
  display.println(F("Device Information"));
  display.println(F("------------------"));
  
  if (deviceConnected) {
    String manufacturer = itla.readAEAString(ITLA_REG_MANUF);
    String model = itla.readAEAString(ITLA_REG_MODEL);
    
    display.print(F("Mfg: "));
    display.println(manufacturer);
    display.print(F("Model: "));
    display.println(model);
    display.print(F("Temp: "));
    display.print(currentTemp, 1);
    display.println(F("C"));
  } else {
    display.println(F("No device connected"));
  }
  
  display.println();
  display.println(F("UP: Back to menu"));
}

void displayLaserControl() {
  display.setCursor(0, 0);
  display.println(F("Laser Control"));
  display.println(F("-------------"));
  
  if (!deviceConnected) {
    display.println(F("No device connected"));
    return;
  }
  
  display.print(F("Status: "));
  switch (laserState) {
    case LASER_ON:
      display.println(F("ON"));
      break;
    case LASER_OFF:
      display.println(F("OFF"));
      break;
    default:
      display.println(F("UNKNOWN"));
      break;
  }
  
  display.println();
  display.println(F("OK: Toggle Laser"));
  display.println(F("UP: Back to menu"));
  
  if (laserState == LASER_ON) {
    display.println();
    display.println(F("WARNING: Laser is ON!"));
  }
}

void displayPowerSettings() {
  display.setCursor(0, 0);
  display.println(F("Power Settings"));
  display.println(F("--------------"));
  
  display.print(F("Current: "));
  display.print(currentPower, 1);
  display.println(F(" dBm"));
  
  display.print(F("Setpoint: "));
  display.print(powerSetpoint, 1);
  display.println(F(" dBm"));
  
  display.println();
  display.println(F("INC/DEC: Adjust"));
  display.println(F("OK: Set Power"));
  display.println(F("UP: Back"));
}

void displayFrequencySettings() {
  display.setCursor(0, 0);
  display.println(F("Frequency Settings"));
  display.println(F("------------------"));
  
  display.print(F("Current: "));
  display.print(currentFreq, 2);
  display.println(F(" THz"));
  
  display.print(F("Setpoint: "));
  display.print(freqSetpoint, 2);
  display.println(F(" THz"));
  
  display.println();
  display.println(F("INC/DEC: Adjust"));
  display.println(F("OK: Set Frequency"));
  display.println(F("UP: Back"));
}

void displayTemperatureMonitor() {
  display.setCursor(0, 0);
  display.println(F("Temperature Monitor"));
  display.println(F("-------------------"));
  
  if (deviceConnected) {
    display.setTextSize(2);
    display.print(currentTemp, 1);
    display.println(F(" C"));
    
    display.setTextSize(1);
    display.println();
    
    // Temperature status
    if (currentTemp > 70) {
      display.println(F("WARNING: HIGH TEMP"));
    } else if (currentTemp < -10) {
      display.println(F("WARNING: LOW TEMP"));
    } else {
      display.println(F("Temperature OK"));
    }
  } else {
    display.println(F("No device connected"));
  }
  
  display.println(F("UP: Back to menu"));
}

void displayStatusMonitor() {
  display.setCursor(0, 0);
  display.println(F("Status Monitor"));
  display.println(F("--------------"));
  
  if (deviceConnected) {
    // Read status registers
    uint16_t statusF = itla.readRegister(ITLA_REG_STATUSF);
    uint16_t statusW = itla.readRegister(ITLA_REG_STATUSW);
    
    display.print(F("Fatal: 0x"));
    display.println(statusF, HEX);
    display.print(F("Warn:  0x"));
    display.println(statusW, HEX);
    
    if (statusF == 0 && statusW == 0) {
      display.println(F("All systems OK"));
    } else {
      display.println(F("Check status!"));
    }
  } else {
    display.println(F("No device connected"));
  }
  
  display.println(F("UP: Back to menu"));
}

void displayAdvancedSettings() {
  display.setCursor(0, 0);
  display.println(F("Advanced Settings"));
  display.println(F("-----------------"));
  display.println(F("Feature coming soon"));
  display.println();
  display.println(F("UP: Back to menu"));
}

void displayConfirmation(const char* message) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(message);
  display.display();
  delay(1500);
}