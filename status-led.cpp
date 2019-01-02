#include "status-led.h"

void StatusLed::setPin(uint8_t pin) {
  _pin = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void StatusLed::turnOn() {
  digitalWrite(_pin, LOW);
}

void StatusLed::turnOff() {
  digitalWrite(_pin, HIGH);
}
