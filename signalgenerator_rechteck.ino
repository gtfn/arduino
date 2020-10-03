// Simpler Rechteckgenerator an Pin 10
// Frequenz ueber Encoder einstellbar

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(3, 4);
//   avoid using pins with LEDs attached

unsigned int frequenz = 1000;
unsigned int f_min = 30;
unsigned int f_max =65535;

void setup() {
  // put your setup code here, to run once:
  //tone(10, frequenz);
}

long oldPosition  = -999;

void loop() {
  // put your main code here, to run repeatedly:
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    //Serial.println(newPosition);
    tone(10, constrain(newPosition, f_min, f_max));
  }
}
