#include <Arduino.h>

#ifndef onewire_client_h
#define onewire_client_h

// Third party libraries.
#include <DallasTemperature.h>  // Library for the Maxim (previously Dallas) DS18B20 (and similar) temperature ICs : https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <OneWire.h>            // Library for Dallas/Maxim 1-Wire Chips: https://github.com/PaulStoffregen/OneWire

class OneWireClient {
 public:
  void setPin(uint8_t pin);
  bool isConnected();
  float getTemperature();

 private:
  OneWire* _oneWire;
  DallasTemperature _sensor;
};

#endif /* onewire_client_h */