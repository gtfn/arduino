/* sketch fuer CAN-modul mit 8MHz quarz MCP2515 library v1.1 von
   nano     CAN-modul                    https://github.com/Flori1989/MCP2515_lib
   5V       VCC                 Erfassung Vor/Ruecklauftemperaturen Heizung
   GND      GND                 Nano als uController
   D10      CS                  CAN Byte 6 Vorlauftemp.
   D11      SI                  CAN Byte 7 Ruecklauftmp.
   D12      SO                  alle 5 min. senden
   D13      SCK
   D2       INT
*/
// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>
// Include the libs for DS18 sensor
#include <OneWire.h>
#include <DallasTemperature.h>
// Oled Display
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

// Sensors DS18 at pin3
#define ONE_WIRE_BUS 3
// setup onewire instance
OneWire oneWire(ONE_WIRE_BUS);
// Pass reference to Dallas Temp
DallasTemperature sensors(&oneWire);

MCP_CAN CAN0(10);             // Set CS to pin 10

// Konstanten
const long intervallCAN = 300000; // sendeintervall CAN 5 min.
const long intervallDS18 = 30000; //sendeintervall DS18/oled 30 sek.
// Variablen
unsigned long prevMillis[] = {0, 0}; // speichere letzte zeit
uint8_t anzahlsensor = 0;     // DS18B20 - Anzahl gefundener Sensoren
uint8_t maxanzahl = 2;        // nur 2 sensoren Flow/Return
int resolution = 9;           // sensor resolution
long unsigned int rxId;       // Variablen 
unsigned char len = 0;        // fuer
unsigned char rxBuf[8];       // CAN-Bus

using MeinArrayType = unsigned char[8];
/****************************************************************/
void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000L);
  sensors.begin();
  sensors.setResolution(resolution);
  anzahlsensor = sensors.getDS18Count();
  if (anzahlsensor > maxanzahl)
  {
    anzahlsensor = maxanzahl;  // check and limit sensorcount
  }
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures(); // Send the command to get temperatures
  // init can bus, baudrate: 125k, ACHTUNG: auf 8 oder 16MHz Quarz aufpassen!
  if (CAN0.begin(CAN_125KBPS, MCP_8MHz) == CAN_OK) Serial.print("can init ok!!\r\n");
  else Serial.print("Can init fail!!\r\n");
  pinMode(2, INPUT);                            // Setting pin 2 for /INT input

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(Adafruit5x7);
  //oled.clear();
  oled.set2X();
  oled.print("OK");
  //delay(2000);
  //oled.clear();
}


MeinArrayType dtmp {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t can_id[] = { 0x100, 0x200, 0x300, 0x400, 0x500 }; // different can id's
/****************************************************************/
void loop()
{
  if (!digitalRead(2))             // If pin 2 is low, read receive buffer
  {
    CAN0.readMsgBuf(&len, rxBuf);  // Read data: len = data length, buf = data byte(s)
    rxId = CAN0.getCanId();        // Get message ID
    if (rxId == 0x110)
    {
      // send data:  id = 0x500, standard frame, data len = 8, stmp: data buf
      send_can(can_id[4], dtmp);
    }
  }
  unsigned long jetztMillis = millis();
  if (jetztMillis - prevMillis[0] >= intervallCAN)
  {
    prevMillis[0] = jetztMillis;
    // send data:  id = 0x500, standard frame, data len = 8, stmp: data buf
    //send_can(can_id[4], dtmp);
  }
  jetztMillis = millis();
  if (jetztMillis - prevMillis[1] >= intervallDS18)
  {
    prevMillis[1] = jetztMillis;
    read_DS18();
    printValues();
    sensors.requestTemperatures();
  }
} //end loop

/****************************************************************/
// Funktionen

void read_DS18()
{
  for (uint8_t i = 0; i < anzahlsensor; i++)
  {
    dtmp[i + 6] = sensors.getTempCByIndex(i); // Wert lesen und in dtmp schreiben
  }
}
/****************************************************************/
void send_can(uint16_t id, MeinArrayType &data) // per Referenz
{
  CAN0.sendMsgBuf(id, 0, 8, data);
}
/****************************************************************/
void printValues()     // Werte auf Oled ausgeben
{
  oled.clear();
  oled.println("Vorlauf:");
  oled.print(dtmp[6]);
  oled.println(" *C");
  oled.println("Ruecklauf:");
  oled.print(dtmp[7]);
  oled.println(" *C");
}
