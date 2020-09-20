#include <Wire.h>
// #include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

/* #define BME_SCK 13
   #define BME_MISO 12
   #define BME_MOSI 11
   #define BME_CS 10
*/

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
unsigned long delayTime;
// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 5000;           // interval at which to blink (milliseconds)

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Wire.setClock(400000L);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.set2X();
  bme.begin(0x76);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  oled.clear();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
      printValues();
    } else {
      ledState = LOW;
    }
  }
}


void printValues() {
  //Serial.print("Temperature = ");
  oled.print(bme.readTemperature());
  oled.println(" *C");

  //Serial.print("Pressure = ");

  //Serial.print(bme.readPressure() / 100.0F);
  //Serial.println(" hPa");

  //Serial.print("Approx. Altitude = ");
  //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  //Serial.println(" m");

  //Serial.print("Humidity = ");
  oled.print(bme.readHumidity());
  oled.println(" %");
}
