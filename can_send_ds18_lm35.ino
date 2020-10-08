
// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>
// Include the libs for DS18
#include <OneWire.h>
#include <DallasTemperature.h>

// Sensors at pin4
#define ONE_WIRE_BUS 4
// setup onewire instance
OneWire oneWire(ONE_WIRE_BUS);
// Pass reference to Dallas Temp
DallasTemperature sensors(&oneWire);

MCP_CAN CAN0(10);                                      // Set CS to pin 10

int LM35 = A0; // LM35 an A0

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  // init can bus, baudrate: 500k
  if (CAN0.begin(CAN_500KBPS) == CAN_OK) Serial.print("can init ok!!\r\n");
  else Serial.print("Can init fail!!\r\n");
}

unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};
void loop()
{
  sensors.requestTemperatures();
  read_LM35();
  // send data:  id = 0x00, standard frame, data len = 8, stmp: data buf
  CAN0.sendMsgBuf(0x00, 0, 8, stmp);
  delay(100);                       // send data per 100ms
}


// Funktionen
void read_LM35() {
  int analoginval;
  analoginval = analogRead(LM35); // Sensor an A0 einlesen
  stmp[7] = (125 * analoginval) >> 8 ; // Wert berechnen und in stmp schreiben
}

void read_DS18() {
  for (uint8_t i = 0; i < sensors.getDeviceCount(); i++) {
    stmp[i] = sensors.getTempCByIndex(i); // Wert lesen und in stmp schreiben
  }
}
