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

// initialize oled 
void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  | Concept                | Meaning                                                                      |
| ---------------------- | ---------------------------------------------------------------------------- |
| `Adafruit_SSD1306`     | Class/blueprint for OLED                                                     |
| `display`              | Object representing your actual OLED                                         |
| `display.begin()`      | Initialize OLED, start communication                                         |
| `0x3C`                 | I²C address of the OLED                                                      |
| `SSD1306_SWITCHCAPVCC` | Use the **OLED’s internal charge pump** to generate required display voltage |

menuIndex = (menuIndex - 1 + mainMenuCount) % mainMenuCount; wraparound function basically 
it makes like if you are at position 0 and you press up button it will go to last position
if you are at last position and you press down button it will go to position 0
 0 1 2   for 0  if u press up it will go to 2
 0 1 2   for 2  if u press down it will go to 1
 0 1 2  for 1 if u press up it will go to 0 
