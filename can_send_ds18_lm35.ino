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

// Sensors DS18 at pin4
#define ONE_WIRE_BUS 4
// setup onewire instance
OneWire oneWire(ONE_WIRE_BUS);
// Pass reference to Dallas Temp
DallasTemperature sensors(&oneWire);

MCP_CAN CAN0(10);                                      // Set CS to pin 10

// Konstanten
const long interval = 1000;   // sendeintervall
// Variablen
unsigned long prevMillis = 0; // speichere letzte zeit
byte State = LOW;             // Status speichern
int resolution = 9;           // sensor resolution

uint16_t can_id[] = { 0x10, 0x20, 0x30, 0x40 }; // different can id's
int LM35 = A0; // LM35 an A0

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  sensors.setResolution(resolution);
  // init can bus, baudrate: 500k
  if (CAN0.begin(CAN_500KBPS, MCP_8MHz) == CAN_OK) Serial.print("can init ok!!\r\n");
  else Serial.print("Can init fail!!\r\n");
}
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char dtmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void loop()
{
  unsigned long jetztMillis = millis();
  if (jetztMillis - prevMillis >= interval) {
    prevMillis = jetztMillis;
    if (State == LOW) {
      State = HIGH;
      sensors.requestTemperatures();
    } else {
      State = LOW;
      read_LM35();
      read_DS18();
      // send data:  id = 0x10, standard frame, data len = 8, stmp: data buf
      //CAN0.sendMsgBuf(0x10, 0, 8, stmp);
      //CAN0.sendMsgBuf(can_id[0], 0, 8, stmp);
      send_can(can_id[0], stmp);
    }
  }
} //end loop


// Funktionen
void read_LM35() {
  stmp[7] = (125 * (analogRead(LM35))) >> 8 ; // Wert berechnen, in stmp schreiben
}

void read_DS18() {
  for (uint8_t i = 0; i < sensors.getDS18Count(); i++) {
    stmp[i] = sensors.getTempCByIndex(i); // Wert lesen und in stmp schreiben
  }
}

void send_can(int i, char data) {
  CAN0.sendMsgBuf(can_id[i], 0, 8, data);
}
