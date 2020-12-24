/* sketch fuer CAN-modul mit 8MHz quarz  MCP2515 library v1.1 von
   nano     CAN-modul                    https://github.com/Flori1989/MCP2515_lib
   5V       VCC                          hd44780 lib
   GND      GND                          Created by Bill Perry 2016-07-02
   D10      CS                           https://github.com/duinoWitchery/hd44780/releases
   D11      SI
   D12      SO
   D13      SCK
   D2       INT
*/
// demo: CAN-BUS Shield, receive data
#include <mcp_can.h>
#include <SPI.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
// declare the lcd object for auto i2c address location
hd44780_I2Cexp lcd;

// LCD geometry
const int LCD_COLS = 20; // 20x4 Display
const int LCD_ROWS = 4;

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

MCP_CAN CAN0(10);                               // Set CS to pin 10


void setup()
{
  /*int status;
    status = lcd.begin(LCD_COLS, LCD_ROWS);
    if(status) // non zero status means it was unsuccesful
    {
    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(status); // does not return
    }*/
  lcd.begin(LCD_COLS, LCD_ROWS);
  // Print a message to the LCD
  // lcd.print("Hello, World!");
  // delay(500); lcd.clear();
  Serial.begin(115200);
  CAN0.begin(CAN_500KBPS, MCP_8MHz);            // init can bus : baudrate = 500k, 8MHz
  pinMode(2, INPUT);                            // Setting pin 2 for /INT input
  Serial.println("MCP2515 Library Receive Example...");
}

void loop()
{
  if (!digitalRead(2))                        // If pin 2 is low, read receive buffer
  {
    CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    rxId = CAN0.getCanId();                    // Get message ID
    Serial.print("ID: ");
    Serial.print(rxId, HEX);
    Serial.print("  Data: ");
    for (int i = 0; i < len; i++)             // Print each byte of the data
    {
      if (rxBuf[i] < 0x10)                    // If data byte is less than 0x10, add a leading zero
      {
        Serial.print("0");
      }
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    if (rxId == 0x00) {
      LCD_write_20x4();
    } // end if rxId
  } // end if digitalRead
} // end loop

// Funktionen
void LCD_write_20x4() {                         // 2 columns, 4 rows
  for (uint8_t i = 0; i < len; i++) {
    if (i < 4) {
      lcd.setCursor(0, i);
    } else {
      lcd.setCursor(4, i - 4);
    }
    lcd.print(rxBuf[i], DEC);
  }
}

void LCD_write_20x2() {                         // 4 columns, 2 rows
  for (uint8_t i = 0; i < len; i++) {
    if (i < 4) {
      lcd.setCursor(i * 4, 0);
    } else {
      lcd.setCursor((i - 4) * 4, 1);
    }
    lcd.print(rxBuf[i], DEC);
  }
}
