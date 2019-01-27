#include "onewire-client.h"

void OneWireClient::setPin(uint8_t pin) {
  _oneWire = new OneWire(pin);
  _sensor.setOneWire(_oneWire);
  _sensor.begin();
}

bool OneWireClient::isConnected() {
  float temperature = getTemperature();
  return temperature != DEVICE_DISCONNECTED_C;
}

float OneWireClient::getTemperature() {
  _sensor.requestTemperatures();
  return _sensor.getTempCByIndex(0);
}
