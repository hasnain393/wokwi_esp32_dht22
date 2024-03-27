#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "Configuration.h" // AWS IoT credentials and WiFi settings
#include <ArduinoJson.h>

// Sensor setup
#define SENSOR_PIN 5
#define SENSOR_TYPE DHT11
DHT climateSensor(SENSOR_PIN, SENSOR_TYPE);

// MQTT topics
#define MQTT_TOPIC_PUBLISH "esp32/pub"
#define MQTT_TOPIC_SUBSCRIBE "esp32/sub"

// WiFi and MQTT client setup
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

void connectToWiFi() {
  Serial.print("Trying to Connect to Internet");
  WiFi.begin(NETWORK_USER, NETWORK_SECRET);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(450);
    Serial.print("*");
  }
  
  Serial.println(" Connected!");
}

void initializeSecureConnection() {
  secureClient.setCACert(AWS_CA);
  secureClient.setCertificate(AWS_THING_CERTIFICATE);
  secureClient.setPrivateKey(AWS_SECRET_KEY);
  
  mqttClient.setServer(AWS_CORE_THING, 8883);
  mqttClient.setCallback(messageReceived);
}

void connectToMQTT() {
  if (mqttClient.connected()) return;
  
  Serial.println("Trying to Connect to MQTT Broker...");
  while (!mqttClient.connected()) {
    if (mqttClient.connect(My_THING_NAME)) {
      Serial.println("MQTT Broker Connected.");
      mqttClient.subscribe(MQTT_TOPIC_SUBSCRIBE);
    } else {
      Serial.println("MQTT Broker Unable to Connect. Reattempt in 5 seconds...");
      delay(5000);
    }
  }
}

void publishSensorData(float humidity, float temp, float co2Level) {
  StaticJsonDocument<256> doc;
  doc["humidity"] = humidity;
  doc["temperature"] = temp;
  doc["co2"] = co2Level;
  
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  
  if (mqttClient.publish(MQTT_TOPIC_PUBLISH, jsonBuffer)) {
    Serial.println("Data Published.");
  } else {
    Serial.println("Publish Failed.");
  }
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(74880);
  climateSensor.begin();
  connectToWiFi();
  initializeSecureConnection();
  connectToMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }

  mqttClient.loop();
  
  float humidity = climateSensor.readHumidity();
  float temperature = climateSensor.readTemperature();
  float co2Level = random(400, 5000); // Simulated CO2 level
  
  if (!isnan(humidity) && !isnan(temperature)) {
    Serial.printf("Humidity: %.1f%%, Temp: %.1f°C, CO2: %.0f ppm\n", humidity, temperature, co2Level);
    publishSensorData(humidity, temperature, co2Level);
  } else {
    Serial.println("Failed to read from climate sensor.");
  }
  
  delay(2000); // Data send interval
}
