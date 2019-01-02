// Platform libraries.
#include <Arduino.h>            // To add IntelliSense for platform constants.
#include <ESP8266WebServer.h>   // To operate as a webserver.
#include <ESP8266WiFi.h>        // To connect to the WiFi network.

// Third party libraries.
#include <DHTesp.h>             // To read DHT22 sensor values: https://github.com/beegee-tokyo/DHTesp

// My classes.
#include "magicmirror-client.h"; // To manage the communication with the MagicMirror.
#include "ifttt-client.h";       // To manage the communication with the IFTTT service.

#include "config.h"  // To store configuration and secrets.

ESP8266WebServer webServer(80);
DHTesp dht;
MagicMirrorClient magicMirror;
IftttClient ifttt;

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

void setup() {
  initSerial();
  initLed();
  initNetwork();
  initWebServer();
  initTemperatureSensor();
  initMagicMirrorClient();
  initIftttClient();

  sendIPAddressNotification();
  turnLedOn();
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
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  Serial.println("DONE.");
}

void initNetwork() {
  Serial.printf("Initializing connection to the %s network with MAC address %s", WIFI_SSID, WiFi.macAddress().c_str());

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  bool ledOn = false;

  while (WiFi.status() != WL_CONNECTED) {
    if (ledOn) {
      turnLedOff();
    } else {
      turnLedOn();
    }
    ledOn = !ledOn;

    delay(500);
    Serial.print(".");
  }

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
  dht.setup(PIN_TEMPERATURE_SENSOR, DHTesp::DHT22);
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
  Serial.println("DONE.");
}

void sendIPAddressNotification() {
  Serial.print("Sending notification about the IP address...");
  String ipAddress = WiFi.localIP().toString();
  String macAddress = WiFi.macAddress();
  String appVersion = String(APP_VERSION);
  ifttt.triggerEvent(IFTTT_WEBHOOK_EVENT_NAME, ipAddress, macAddress, appVersion);
  Serial.println("DONE.");
}

void loop() {
  webServer.handleClient();
}

void onRootRequest() {
  Serial.println("Received HTTP request to /");
  turnLedOff();
  sendResponse(String("OK from ") + WiFi.localIP().toString().c_str());
  turnLedOn();
}

void onTempRequest() {
  Serial.println("Received HTTP request to /temp");
  turnLedOff();
  Measurement measurement = getMeasuredData();
  magicMirror.sendTemperature(measurement.temperature);
  String response = buildTempReponse(measurement);
  sendResponse(response);
  turnLedOn();
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

Measurement getMeasuredData() {
  TempAndHumidity sensorData = dht.getTempAndHumidity();

  Measurement measurement;
  measurement.temperature = sensorData.temperature;
  measurement.humidity = sensorData.humidity;
  measurement.status = dht.getStatusString();
  measurement.dewPoint = dht.computeDewPoint(sensorData.temperature, sensorData.humidity, false);
  measurement.heatIndex = dht.computeHeatIndex(sensorData.temperature, sensorData.humidity, false);
  measurement.perception = dht.computePerception(sensorData.temperature, sensorData.humidity, false);
  measurement.perceptionString = getPerceptionString(measurement.perception);

  ComfortState cs;
  measurement.comfortRatio = dht.getComfortRatio(cs, sensorData.temperature, sensorData.humidity, false);
  measurement.comfortStateString = getComfortStateString(cs);

  return measurement;
}

void turnLedOn() {
  digitalWrite(PIN_LED, LOW);
}

void turnLedOff() {
  digitalWrite(PIN_LED, HIGH);
}

const char* getPerceptionString(byte perception) {
  switch (perception) {
    case Perception_Dry: return "Dry";
    case Perception_VeryComfy: return "Very comfy";
    case Perception_Comfy: return "Comfy";
    case Perception_Ok: return "Ok";
    case Perception_UnComfy: return "Uncomfy";
    case Perception_QuiteUnComfy: return "Quite uncomfy";
    case Perception_VeryUnComfy: return "Very uncomfy";
    case Perception_SevereUncomfy: return "Severy uncomfy";
  }
}

const char* getComfortStateString(ComfortState comfortState) {
  switch (comfortState) {
    case Comfort_OK: return "Ok";
    case Comfort_TooHot: return "Too hot";
    case Comfort_TooCold: return "Too cold";
    case Comfort_TooDry: return "Too dry";
    case Comfort_TooHumid: return "Too humid";
    case Comfort_HotAndHumid: return "Hot and humid";
    case Comfort_HotAndDry: return "Hot and dry";
    case Comfort_ColdAndHumid: return "Cold and humid";
    case Comfort_ColdAndDry: return "Cold and dry";
  }
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