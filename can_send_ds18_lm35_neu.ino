/* sketch fuer CAN-modul mit 8MHz quarz  MCP2515 library v1.1 von
   nano     CAN-modul                    https://github.com/Flori1989/MCP2515_lib
   5V       VCC
   GND      GND
   D10      CS
   D11      SI
   D12      SO
   D13      SCK
   D2       INT
*/
// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>
// Include the libs for DS18 sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// Sensors DS18 at pin3
#define ONE_WIRE_BUS 3
// setup onewire instance
OneWire oneWire(ONE_WIRE_BUS);
// Pass reference to Dallas Temp
DallasTemperature sensors(&oneWire);

MCP_CAN CAN0(10);              // Set CS to pin 10

// Konstanten
const long intervall = 2000;   // sendeintervall
// Variablen
unsigned long prevMillis = 0; // speichere letzte zeit
uint8_t anzahlsensor = 0;     // DS18B20 - Anzahl
int resolution = 9;           // sensor resolution
// analoger Temperatursensor
int LM35 = A0;

using MeinArrayType = unsigned char[8];

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  sensors.setResolution(resolution);
  anzahlsensor = sensors.getDS18Count();
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures(); // Send the command to get temperatures
  // init can bus, baudrate: 500k, ACHTUNG: auf 8 oder 16MHz Quarz aufpassen!
  if (CAN0.begin(CAN_500KBPS, MCP_16MHz) == CAN_OK) Serial.print("can init ok!!\r\n");
  else Serial.print("Can init fail!!\r\n");
}


MeinArrayType stmp {0, 0, 0, 0, 0, 0, 0, 0};
MeinArrayType dtmp {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t can_id[] = { 0x00, 0x01, 0x02, 0x03, 0x04 }; // different can id's
// uint16_t can_id[] = { 0x00, 0x01, 0x02, 0x03, 0x04 };

void loop()
{
  unsigned long jetztMillis = millis();
  if (jetztMillis - prevMillis >= intervall) {
    prevMillis = jetztMillis;
    //Serial.print("Sending Data...\r\n");
    read_LM35();
    read_DS18();
    // send data:  id = 0x10, standard frame, data len = 8, stmp: data buf
    //CAN0.sendMsgBuf(0x10, 0, 8, stmp);
    send_can(can_id[1], stmp);
    send_can(can_id[0], dtmp);
    sensors.requestTemperatures();
  }
} //end loop


// Funktionen
void read_LM35() {
  stmp[7] = (125 * (analogRead(LM35))) >> 8 ; // Wert berechnen, in stmp schreiben
}

void read_DS18() {
  for (uint8_t i = 0; i < anzahlsensor; i++) {
    dtmp[i] = sensors.getTempCByIndex(i); // Wert lesen und in dtmp schreiben
  }
}

void send_can(uint16_t i, MeinArrayType &data) { // per Referenz
  CAN0.sendMsgBuf(i, 0, 8, data);
}

/*
  void send_can(uint16_t i, MeinArrayType &data) { // per Referenz

  unsigned char tempData[8];
  for (int cnt = 0; cnt < 8; cnt++)
  {
    tempData[cnt] = data[cnt];
  }

  if (CAN0.sendMsgBuf(i, 0, 8, tempData) == CAN_OK) Serial.print("Buf sent!\r\n");
  else Serial.print("Buf send failed!\r\n");
  }*/
