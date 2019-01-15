#include <Arduino.h>

#ifndef ota_updater_h
#define ota_updater_h

class OTAUpdater {
 public:
  void initialize(const char* hostName, const char* password);
  void handleUpdateRequests();
};

#endif /* ota_updater_h */