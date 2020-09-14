
/*

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

   Neue Version meines LCD Sketches
   Zeigt abwechselnd Temperatur/Feuchte von 2 DHT22 Fuehlern
   ohne Nachkommastellen um Platz auf Display zu sparen
*/

// include the library code:
#include <LiquidCrystal.h>
#include "DHT.h"

//DHT Array erstellen
DHT dht[2];

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Variablen
int dht_State = LOW;     // used to set the dht_state
unsigned long previousMillis = 0; // speichere letzte zeit
const long interval = 2000;    // interval des anzeigewechsels
int humidity;
int temperature;
byte sensor;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  dht[0].setup(8); //erster dht pin 8
  dht[1].setup(9); //zweiter dht pin 9
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if dht0 is active change to dht1 and vice-versa:
    if (dht_State == LOW) {
      dht_State = HIGH;
      sensor = 1;
    } else {
      dht_State = LOW;
      sensor = 0;
    }
    
      delay(dht[sensor].getMinimumSamplingPeriod());
      humidity = int(dht[sensor].getHumidity()); // Nachkommastellen abschneiden
      temperature = int(dht[sensor].getTemperature()); // Nachkommastellen abschneiden

      lcd.setCursor(0, 0);
      lcd.print("Temp.:");

      lcd.setCursor(0, 1);
      lcd.print("Feuchte:");

      lcd.setCursor(9, 0);
      lcd.print(temperature);
      lcd.print(char(223)), lcd.print("C");

      lcd.setCursor(9, 1);
      lcd.print(humidity);
      lcd.print("%");
      lcd.setCursor(15, 0); // Cursor ans Ende der ersten Zeile
      lcd.print(sensor + 1); // Sensornummer anzeigen
  }
