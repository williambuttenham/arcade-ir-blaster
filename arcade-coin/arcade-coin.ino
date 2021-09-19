#include "Keyboard.h"

const byte interruptPin = 7;
boolean insert = false;
volatile int pulse = 0;

void setup() {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), coinInterrupt, RISING);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (insert) {
    insert = false;
    Keyboard.press('l');
    delay(100);
    Keyboard.releaseAll();
    delay(500);
  }
}

//interrupt
void coinInterrupt() {
  pulse++ ;
  insert = true;
}
