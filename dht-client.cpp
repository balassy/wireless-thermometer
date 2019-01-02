#include "dht-client.h"

void DhtClient::setPin(uint8_t pin) {
  _dht.setup(pin, DHTesp::DHT22);
}

Measurement DhtClient::getMeasuredData() {
  TempAndHumidity sensorData = _dht.getTempAndHumidity();

  Measurement measurement;
  measurement.temperature = sensorData.temperature;
  measurement.humidity = sensorData.humidity;
  measurement.status = _dht.getStatusString();
  measurement.dewPoint = _dht.computeDewPoint(sensorData.temperature, sensorData.humidity, false);
  measurement.heatIndex = _dht.computeHeatIndex(sensorData.temperature, sensorData.humidity, false);
  measurement.perception = _dht.computePerception(sensorData.temperature, sensorData.humidity, false);
  measurement.perceptionString = _getPerceptionString(measurement.perception);

  ComfortState cs;
  measurement.comfortRatio = _dht.getComfortRatio(cs, sensorData.temperature, sensorData.humidity, false);
  measurement.comfortStateString = _getComfortStateString(cs);

  return measurement;
}

const char* DhtClient::_getPerceptionString(byte perception) {
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

const char* DhtClient::_getComfortStateString(ComfortState comfortState) {
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