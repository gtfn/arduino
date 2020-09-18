/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 (for MKRZero SD: SDCARD_SS_PIN)

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*** erweitert fuer die Temp.messung/Logging mit TMP36, RTC DS1307
*** und einem microSD card adapter von debo

*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

RTC_DS1307 rtc;

//TMP36 Pin Variables
int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
//the resolution is 10 mV / degree centigrade with a
//500 mV offset to allow for negative temperatures

// the digital pins that connect to the LED
int greenLED = 8;

const int chipSelect = 10;

void setup() {
  // set up the LCD's number of rows and columns:
  lcd.begin(16, 2);
  pinMode(greenLED, OUTPUT); // als ausgang setzen

  // Open serial communications and wait for port to open:
  /*  Serial.begin(9600);
      while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
  */

  //starte rtc
  if (! rtc.begin()) {
    lcd.print(F("Couldn't find RTC"));
    while (1);
  }

  lcd.print(F("Initialize Card"));
  delay(2000); lcd.clear();

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    lcd.print(F("Failed/no card?"));
    // don't do anything more:
    while (1);
  }
  lcd.setCursor(0, 0);
  lcd.print(F("Card init ok"));
  delay(2000);
  lcd.clear();
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";
  // rtc abfragen
  DateTime now = rtc.now();
  //getting the voltage reading from the temperature sensor
  int reading = analogRead(sensorPin);
  // converting that reading to voltage
  float voltage = (reading * 5.0) / 1024.0;
  // calculate the temperature
  float temperatureC = (voltage - 0.5) * 100 ;
  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  dataString += String(temperatureC);

      // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
      digitalWrite(greenLED, HIGH);
      now = rtc.now();
      dataFile.print(now.day(), DEC);
      dataFile.print('.');
      dataFile.print(now.month(), DEC);
      dataFile.print('.');
      dataFile.print(now.year(), DEC);
      dataFile.print(",");
      if (now.hour() < 10) {
        dataFile.print("0");
      }
      dataFile.print(now.hour(), DEC);
      dataFile.print(':');
      if (now.minute() < 10) {
        dataFile.print("0");
      }
      dataFile.print(now.minute(), DEC); dataFile.print(",");
      dataFile.println(dataString);
      dataFile.close();
      digitalWrite(greenLED, LOW);
    }
    // if the file isn't open, pop up an error:
    else {
      lcd.print(F("error op. file"));    
  }
  // print to the lcd
  lcd.setCursor(0, 0);
  lcd.print(temperatureC);

  delay(60000);
}
