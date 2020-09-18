/*  basiert auf einem sketch von adafruit und mehreren examples
Display ist ueber i2c backpack angeschlossen
  The circuit:
 * 5V to Arduino 5V pin
 * GND to Arduino GND pin
 * CLK to Analog #5
 * DAT to Analog #4
 */

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#include <OneWire.h>
#include <DallasTemperature.h>

// include the library code:
#include "Wire.h"
#include "LiquidCrystal.h"

// these constants won't change.  But you can change the size of
// your LCD using them:
const int numRows = 4;
const int numCols = 16;
const int Button = 7;  //input pin fuer schalter lcd backlight
int val = 0; //variable status input pin

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
// das hier habe ich so gelassen
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress addr;

// Connect via i2c, default address #0 (A0-A2 not jumpered)
LiquidCrystal lcd(0);
LiquidCrystal lcd1(1);

// Define the RTC
RTC_DS1307 rtc;

void setup(void)
{
  // set up the LCD's number of rows and columns:
  lcd.begin(numCols, numRows);
  lcd1.begin(20, 4);
  lcd.setBacklight(HIGH);      // Turn on the backlight
  lcd1.setBacklight(HIGH);
  // Start up the library
  sensors.begin();
  rtc.begin();
  // zeigt beim Start kurz die Uhrzeit an
  DateTime now = rtc.now();
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  lcd1.print(F("Ni/CrNi Thermometer"));
  delay(5000);
  //lcd1.setBacklight(LOW);
  lcd.setBacklight(LOW);
  lcd1.setBacklight(LOW);     // Turn off the backlight
  pinMode(Button, INPUT); // modus=eingang
}

void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  // schreibe die Temperaturen aufs display
  for (uint8_t s = 0; s < sensors.getDeviceCount(); s++) {
    if (s < 2) {
      lcd.setCursor(0, s);  // first 2 linesof 16x4 display
    }
    else {
      lcd.setCursor(-4, s); // lines 3 and 4 of 16x4 display
    }
    lcd.print(s + 1); lcd.print(":");
    lcd.print(sensors.getTempCByIndex(s), 1);
    lcd.print(char(223)); lcd.print("C ");
  }
  // zeigt auf Display 2 die Uhrzeit an
  DateTime now = rtc.now();
  lcd1.setCursor(0, 1);
  lcd1.print(now.hour(), DEC);
  lcd1.print(':');
  lcd1.print(now.minute(), DEC);
  lcd1.print("  ");
  //lcd1.print(':');
  //lcd1.print(now.second(), DEC);
  
  val = digitalRead(Button); //lies input und speichere
  //check input is HIGH-backlight ein
  if (val == HIGH) {
    lcd.setBacklight(HIGH);      // Turn on the backlight
    lcd1.setBacklight(HIGH);
  } else {
    lcd.setBacklight(LOW);
    lcd1.setBacklight(LOW);
  }
}
