/*
  The circuit:
   TMP36 -> AnalogIn A0
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 (for MKRZero SD: SDCARD_SS_PIN)
*** erweitert fuer die Temp.messung/Logging mit TMP36, RTC DS1307
*** und einem microSD card adapter

*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>                      //https://github.com/adafruit/RTClib
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

RTC_DS1307 rtc;

// Variables will change:
int sensorPin = 0;                       //the TMP36's Vout
//the resolution is 10 mV / degree centigrade with a
//500 mV offset to allow for negative temperatures
float temperatureC = 0;                  //temperaturwert
//unsigned long previousMillis = 0;        // will store last time SD/lcd was updated
uint32_t previousMillis[] = {0, 0};

// constants won't change:
const int chipSelect = 10;               // SD Card writer
const long intervall = 300000;           // interval at which to write (milliseconds)
const int greenLED = 8;                  // the LED pin

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(greenLED, OUTPUT);

  // Open serial communications and wait for port to open:
  /*  Serial.begin(9600);
      while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
  */

  if (! rtc.begin()) {                   // starte rtc
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
  //lcd.clear();
  lcd.print(F("Card init ok"));
  delay(2000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now(); // rtc abfragen
  //unsigned long currentMillis = millis();
  uint32_t currentMillis {millis()};
  temperatureC = read_TMP36();

  if (currentMillis - previousMillis[0] >= intervall) {
    previousMillis[0] = currentMillis;                   // last write_SD()
    write_SD(String(temperatureC));
  }
  currentMillis = millis();
  if (currentMillis - previousMillis[1] >= 2000) {
    print_LCD();
    previousMillis[1] = currentMillis;                   // last print_LCD()
  }

  lcd.setCursor(0, 1);
  lcd.print(now.timestamp(DateTime::TIMESTAMP_TIME));   // Timestamp
}

// Funktionen

float read_TMP36() {
  float merk = 0, tempvalC;
  for (byte i = 0; i < 3; i++) {
    int reading = analogRead(sensorPin);
    float voltage = (reading * 5.0) / 1024.0;            // converting that reading to voltage
    float tempvalC = (voltage - 0.5) * 100 ;             // calculate the temperature
    //converting from 10 mv per degree wit 500 mV offset
    //to degrees ((voltage - 500mV) times 100)
    /********************************************/
    merk += tempvalC;
  }
  tempvalC = merk / 3;
  return tempvalC;
}

void write_SD(String dataString) {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    digitalWrite(greenLED, HIGH);
    DateTime now = rtc.now();
    now = rtc.now();
    dataFile.print(now.timestamp(DateTime::TIMESTAMP_FULL));
    dataFile.print(",");
    dataFile.println(dataString);
    dataFile.close();
    digitalWrite(greenLED, LOW);
  }
  else {
    lcd.print(F("error op. file"));
  }
}

void print_LCD() {
  lcd.setCursor(0, 0);
  lcd.print(temperatureC);
  lcd.print(char(223)); lcd.print('C');
}
