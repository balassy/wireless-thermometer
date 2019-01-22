#include <ESP8266HTTPClient.h>  // To send HTTP requests.
#include <WiFiClientSecureBearSSL.h>

#include "thingspeak-client.h"

void ThingSpeakClient::initialize(String apiKey, IftttClient* ifttt) {
  _apiKey = apiKey;
  _ifttt = ifttt;
}

void ThingSpeakClient::setApiKey(String apiKey) {
  _apiKey = apiKey;
}

void ThingSpeakClient::writeField(String field1Value, String field2Value, String statusMessage) {
  String url = String("https://api.thingspeak.com/update");
  Serial.println("ThingSpeakClient: Sending POST request to " + url);

  String requestBody = "api_key=" + _apiKey + "&field1=" + field1Value + "&field2=" + field2Value + "&status=" + statusMessage;
  Serial.println("ThingSpeakClient: HTTP request body: " + requestBody);

  // RootCA certificate in .cer format.
  static const char digicert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j
ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL
MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3
LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug
RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm
+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW
PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM
xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB
Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3
hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg
EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF
MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA
FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec
nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z
eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF
hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2
Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe
vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep
+OkuE6N36B9K
-----END CERTIFICATE-----
)EOF";

  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  BearSSL::X509List cert(digicert);
  client->setTrustAnchors(&cert);
  // client->setInsecure();

  HTTPClient http;
  http.begin(*client, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int statusCode = http.POST(requestBody);

  Serial.printf("ThingSpeakClient: Received HTTP status code: %d\r\n", statusCode);
  if (statusCode != HTTP_CODE_OK) {
    String responseBody = http.getString();
    Serial.println("ThingSpeakClient: Received HTTP response body: " + responseBody);
    _ifttt->triggerEvent(IFTTT_WEBHOOK_EVENT_NAME, "ThingSpeak update failed", "ThingSpeak update failed. HTTP status code: " + String(statusCode) + ". Response body: " + responseBody);
  } else {
    _ifttt->triggerEvent(IFTTT_WEBHOOK_EVENT_NAME, "ThingSpeak updated successfully", "ThingSpeak board updated successfully.");
  }


  http.end();
}