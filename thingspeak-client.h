#include <Arduino.h>

#ifndef thingspeak_client_h
#define thingspeak_client_h

#include "ifttt-client.h"

extern const char* IFTTT_WEBHOOK_EVENT_NAME;

class ThingSpeakClient {
 public:
  void initialize(String apiKey, IftttClient* ifttt);
  void setApiKey(String apiKey);
  void writeField(String field1Value, String field2Value, String statusMessage);

 private:
  String _apiKey;
  IftttClient* _ifttt;
};

#endif /* thingspeak_client_h */