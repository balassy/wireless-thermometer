// Platform libraries.
#include <Arduino.h>           // To add IntelliSense for platform constants.
#include <ESP8266WebServer.h>  // To operate as a webserver.
#include <ESP8266WiFi.h>       // To connect to the WiFi network.
#include <Ticker.h>            // To conveniently manage timer intervals.

// Third-party libraries.
#include <WiFiManager.h>  // To manage network configuration and connection.

// My classes.
#include "dht-client.h"          // To manage the temperature sensor.
#include "ifttt-client.h"        // To manage the communication with the IFTTT service.
#include "magicmirror-client.h"  // To manage the communication with the MagicMirror.
#include "ota-updater.h"         // To manage over-the-air updates of new code.
#include "status-led.h"          // To control the status LED.
#include "thingspeak-client.h"   // To send measured data to the ThingSpeak service.

#include "config.h"  // To store configuration and secrets.

ESP8266WebServer webServer(80);
Ticker timer;
WiFiManager wifiManager;

MagicMirrorClient magicMirror;
IftttClient ifttt;
ThingSpeakClient thingSpeak;
DhtClient dht;
StatusLed led;
OTAUpdater updater;

bool timerTriggerActivated = false;
bool isFirstLoop = true;

void setup() {
  initSerial();
  initLed();
  initNetwork();
  initWebServer();
  initTemperatureSensor();
  initMagicMirrorClient();
  initIftttClient();
  initThingSpeakClient();
  initUpdater();

  initTimer();

  sendStartNotification();
  led.turnOn();
  Serial.printf("Application version: %s\n", APP_VERSION);
  Serial.println("Setup completed.");
}

void initSerial() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing serial connection DONE.");
}

void initLed() {
  Serial.printf("Initializing LED on pin %d...", PIN_LED);
  led.setPin(PIN_LED);
  Serial.println("DONE.");
}

void initNetwork() {
  Serial.printf("Initializing connection to the network with MAC address %s using WiFiManager (SSID: %s)...", WiFi.macAddress().c_str(), WIFI_AP_SSID);
  wifiManager.autoConnect(WIFI_AP_SSID, WIFI_AP_PASSWORD);
  Serial.printf("DONE. IP address: %s, MAC address: %s\n", WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str());
}

void initWebServer() {
  Serial.print("Initializing webserver...");
  webServer.on("/", onRootRequest);
  webServer.on("/temp", onTempRequest);
  webServer.begin();
  Serial.println("DONE.");
}

void initTemperatureSensor() {
  Serial.printf("Initializing temperature sensor on pin %d...", PIN_TEMPERATURE_SENSOR);
  dht.setPin(PIN_TEMPERATURE_SENSOR);
  Serial.println("DONE.");
}

void initMagicMirrorClient() {
  Serial.printf("Initializing MagicMirror client to host %s...", MAGIC_MIRROR_HOST);
  magicMirror.setHostUrl(MAGIC_MIRROR_HOST);
  Serial.println("DONE.");
}

void initIftttClient() {
  Serial.print("Initializing IFTTT client...");
  ifttt.setApiKey(IFTTT_WEBHOOK_API_KEY);

  String deviceName = String(OTA_UPDATE_HOSTNAME) + " (Version: " + APP_VERSION + ", IP: " + WiFi.localIP().toString() + ", MAC: " + WiFi.macAddress() + ")";
  ifttt.setDeviceName(deviceName);

  Serial.println("DONE.");
}

void initThingSpeakClient() {
  Serial.print("Initializing ThingSpeak client...");
  thingSpeak.setApiKey(THINGSPEAK_API_KEY);
  Serial.println("DONE.");
}

void initUpdater() {
  Serial.print("Initializing over-the-air updater...");
  updater.initialize(OTA_UPDATE_HOSTNAME, OTA_UPDATE_PASSWORD, ifttt);
  Serial.println("DONE.");
}

void initTimer() {
  Serial.print("Initializing timer...");
  timer.attach(UPDATE_INTERVAL_SECONDS, onTimerInterval);
  Serial.println("DONE.");
}

void sendStartNotification() {
  Serial.print("Sending notification about the IP address...");
  String ipAddress = WiFi.localIP().toString();
  String macAddress = WiFi.macAddress();

  String message = String("Your device is starting. Reason: ") + ESP.getResetReason() + " (" + ESP.getResetInfo() + ")";
  ifttt.triggerEvent(IFTTT_WEBHOOK_EVENT_NAME, "Starting", message);
  Serial.println("DONE.");
}

void loop() {
  // Updating the targets before anything else to notify the targets that the "loop" is running after the completed "setup".
  if (isFirstLoop) {
    measureAndUpdateTargets();
    isFirstLoop = false;
  }

  webServer.handleClient();
  updater.handleUpdateRequests();

  if (timerTriggerActivated) {
    measureAndUpdateTargets();
  }
}

void onTimerInterval() {
  // IMPORTANT:
  // "It is currently not recommended to do blocking IO operations (network, serial, file) from Ticker callback functions.
  // Instead, set a flag inside the ticker callback and check for that flag inside the loop function."
  // Source: https://github.com/adafruit/ESP8266-Arduino#ticker
  timerTriggerActivated = true;
}

void measureAndUpdateTargets() {
  Serial.print("Timer: ");
  Measurement m = dht.getMeasuredData();

  Serial.print("Sending data to ThingSpeak...");
  thingSpeak.writeField(String(m.temperature), String(m.humidity), String(m.perceptionString) + " " + String(m.comfortStateString));
  Serial.println("DONE.");

  Serial.print("Sending data to MagicMirror...");
  magicMirror.sendTemperature(m.temperature, m.humidity);
  Serial.println("DONE.");

  timerTriggerActivated = false;
  Serial.print("Timer: DONE.");
}

void onRootRequest() {
  Serial.println("Received HTTP request to /");
  led.turnOff();
  sendResponse(String("OK from ") + WiFi.localIP().toString().c_str());
  led.turnOn();
}

void onTempRequest() {
  Serial.println("Received HTTP request to /temp");
  led.turnOff();
  Measurement measurement = dht.getMeasuredData();
  magicMirror.sendTemperature(measurement.temperature, measurement.humidity);
  String response = buildTempReponse(measurement);
  sendResponse(response);
  led.turnOn();
}

void sendResponse(String content) {
  Serial.println("Sending response: " + content);
  webServer.send(200, "text/plain", content);
}

String buildTempReponse(Measurement& measurement) {
  return String("Temperature: ") + measurement.temperature + " C\r\n" +
         "Humidity: " + measurement.humidity + " %\r\n" +
         "Status: " + measurement.status + "\r\n" +
         "Dew point: " + measurement.dewPoint + " C\r\n" +
         "Heat index: " + measurement.heatIndex + "\r\n" +
         "Perception: " + measurement.perception + " - " + measurement.perceptionString + "\r\n" +
         "Comfort: " + measurement.comfortRatio + " - " + measurement.comfortStateString;
}

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}
