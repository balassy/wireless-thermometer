#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp8266.h>  // To communicate with the Blynk service re-using the existing WiFi connection.

#include "blynk-client.h"

void BlynkClient::setAuthToken(const char* authToken) {
  Blynk.config(authToken);
}

void BlynkClient::sendUpdate(float temperature, float humidity) {
  Blynk.virtualWrite(V16, temperature);
  Blynk.virtualWrite(V17, humidity);
}

void BlynkClient::handleLoop() {
  Blynk.run();
}