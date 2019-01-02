#include <Arduino.h>

#ifndef dht_client_h
#define dht_client_h

// Third party libraries.
#include <DHTesp.h>  // To read DHT22 sensor values: https://github.com/beegee-tokyo/DHTesp

struct Measurement {
  float temperature;
  float humidity;
  const char* status;
  float dewPoint;
  float heatIndex;
  byte perception;
  const char* perceptionString;
  float comfortRatio;
  const char* comfortStateString;
};

class DhtClient {
 public:
  void setPin(uint8_t pin);
  Measurement getMeasuredData();

 private:
  DHTesp _dht;
  const char* _getPerceptionString(byte perception);
  const char* _getComfortStateString(ComfortState comfortState);
};

#endif /* dht_client_h */