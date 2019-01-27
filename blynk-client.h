#include <Arduino.h>

#ifndef blynk_client_h
#define blynk_client_h

class BlynkClient {
 public:
  void setAuthToken(const char* authToken);
  void sendUpdate(float temperature);
  void sendUpdate(float temperature, float humidity);
  void handleLoop();
};

#endif /* blynk_client_h */