const char* APP_VERSION = "0.0.1";

#ifndef LED_BUILTIN
#define LED_BUILTIN 16
#endif

const int PIN_LED = LED_BUILTIN;        // The number of the PCB bin to where the status LED is connected.
const int PIN_TEMPERATURE_SENSOR = 14;  // The number of the PCB pin to where the DHT22 sensor is connected.

const char* WIFI_SSID = "TODO_MY_NETWORK_NAME";          // The name of the wireless network to connec to.
const char* WIFI_PASSWORD = "TODO_MY_NETWORK_PASSWORD";  // The password for the wireless network.

const char* IFTTT_WEBHOOK_EVENT_NAME = "TODO_MY_IFTTT_EVENT_NAME";  // The name of the IFTTT event to trigger when the device starts.
const char* IFTTT_WEBHOOK_API_KEY = "TODO_MY_IFTTT_API_KEY";        // The IFTTT API key used to trigger the above event.
