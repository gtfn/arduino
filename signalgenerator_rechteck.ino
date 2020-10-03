// Simpler Rechteckgenerator an Pin 10
// Frequenz ueber Encoder einstellbar

#include <Encoder.h>
#include <Wire.h>
#include "rgb_lcd.h" // Grove i2c lcd

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(3, 4);
//   avoid using pins with LEDs attached

unsigned int frequenz = 1000;
unsigned int f_min = 30;
unsigned int f_max = 65535;

void setup() {
  // put your setup code here, to run once:
  //tone(10, frequenz);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  // Print a message to the LCD.
  lcd.print("hello, world!");

  delay(1000);
}

long oldPosition  = -999;

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    lcd.print(newPosition);
    tone(10, constrain(newPosition, f_min, f_max));
  }
}
