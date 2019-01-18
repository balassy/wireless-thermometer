#include <Arduino.h>

#ifndef ota_updater_h
#define ota_updater_h

#include "ifttt-client.h"

extern const char* IFTTT_OTA_WEBHOOK_EVENT_NAME;

class OTAUpdater {
 public:
  void initialize(const char* hostName, const char* password, IftttClient &ifttt);
  void handleUpdateRequests();
};

#endif /* ota_updater_h */