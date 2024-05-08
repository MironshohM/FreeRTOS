#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi and MQTT server credentials
const char* ssid = "Network";
const char* password = "4467996m";
const char* mqtt_server = "mqtt.iotserver.uz";
const int mqtt_port = 1883;
const char* mqtt_username = "userTTPU";
const char* mqtt_password = "mqttpass";

WiFiClient espClient;
PubSubClient client(espClient);

bool led1_state = false;
bool led2_state = false;

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

const int led_1 = 32;
const int led_2 = 25;

void Task1code( void * parameter );
void Task2code( void * parameter );
void Task3code( void * parameter );
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(115200); 
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);

  xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);                         
  delay(500); 

  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);          
  delay(500); 

  xTaskCreatePinnedToCore(Task3code,"Task3",10000,NULL,1,&Task3,1);          
  delay(500); 
}

void Task1code( void * parameter ){
  Serial.print("Task1 is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led_1, led1_state ? HIGH : LOW);
    delay(500);
  } 
}

void Task2code( void * parameter ){
  Serial.print("Task2 is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    digitalWrite(led_2, led2_state ? HIGH : LOW);
    delay(1000);
  }
}

void Task3code( void * parameter ){
  
  Serial.print("Task3 is running on core ");
  Serial.println(xPortGetCoreID());

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
while (!client.connected()) {
  if (client.connect("ESP32Client123", mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT server");
  } else {
    delay(500);
    Serial.println("Connecting to MQTT server..");
  }
}

  client.subscribe("ttpu/MQTT");

  for(;;){
    client.loop();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message received");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload, length);
  led1_state = doc["led1"];
  led2_state = doc["led2"];
  Serial.print("LED1 state: ");
  Serial.println(led1_state ? "ON" : "OFF");
  Serial.print("LED2 state: ");
  Serial.println(led2_state ? "ON" : "OFF");
}

void loop() {
  
}