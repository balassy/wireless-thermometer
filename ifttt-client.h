#include <Arduino.h>

#ifndef ifttt_client_h
#define ifttt_client_h

class IftttClient {
  public:
    void setApiKey(String apiKey);
    void triggerEvent(String eventName, String value1, String value2, String value3);

  private:
    String _apiKey;
};

#endif /* ifttt_client_h */