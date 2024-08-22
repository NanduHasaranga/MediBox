// Medibox Project
// Name - Hasaranga T.N.
// Index Number - 210210G

// Libraries
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include "DHTesp.h"

// Declare Variables
int servoAngle = 0;
int minAngle = 30;
float controlFactor = 0.75;

// Declare objects
WiFiClient espClient;
PubSubClient mqttClient(espClient);
Servo servo;
char tempAr[6];
DHTesp dhtSensor;

// Define Pins
#define DHT_PIN 15
#define LEFT_LDR 34
#define RIGHT_LDR 35

const int SERVO_PIN = 5;

void setup() {
  Serial.begin(115200);
  analogReadResolution(10);
  servo.attach(SERVO_PIN);
  setupWifi();

    for (int i = 0; i < 180; i++)
    {
        servo.write(i);
        delay(15);
    }
    for (int i = 180; i >= 0; i--)
    {
        servo.write(i);
        delay(15);
    }

  setupMqtt();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
}

void loop() {
  if(!mqttClient.connected()){
    connectToBroker();
  }

   // Reading LDR values
    int left_value = analogRead(LEFT_LDR);
    int right_value = analogRead(RIGHT_LDR);

    // Converting LDR values between 0 - 1
    float left_send = 1 - left_value / 1023.0;
    float right_send = 1 - right_value / 1023.0;

    // Displaying values in Serial Monitor
    Serial.print("Left LDR : ");
    Serial.println(left_send);
    Serial.print("Right LDR : ");
    Serial.println(right_send);
    Serial.print("Motor Angle : ");
    Serial.println(servoAngle);
    Serial.print("Min Angle : ");
    Serial.println(minAngle);
    Serial.print("C.Factor : ");
    Serial.println(controlFactor);

    float max_LDR;
    int max_side;
    int D;

    if (left_send > right_send)
    {
        max_LDR = left_send;
        max_side = 0;
        D = 1.5;
    }
    else
    {
        max_LDR = right_send;
        max_side = 1;
        D = 0.5;
    }

  char bufValue[8]; // Sufficient space for max float value
  char bufSide[2];  // Enough space for single digit and null terminator // 0 = left, 1 = right

  // Format float value into buffer
  dtostrf(max_LDR, 6, 2, bufValue);
  // Convert int to string
  itoa(max_side, bufSide, 10); 

  mqttClient.publish("MEDIBOX-NANDU-MAXLDR", bufValue);
  mqttClient.publish("MEDIBOX-NANDU-MAXSIDE", bufSide);

  servoAngle = min(int(minAngle * D + (180 - minAngle) * max_LDR * controlFactor), 180);
  servo.write(servoAngle);

  updateTemperature();
  Serial.print("Temperature: ");
  Serial.println(tempAr);
  Serial.println();
  mqttClient.publish("MEDIBOX-NANDU-TEMP", tempAr);
  delay(3000);
  mqttClient.loop();
}

void setupMqtt(){
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(receivecallback);
}

void receivecallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message Arrived [");
    Serial.print(topic);
    Serial.print("] ");

    String topics = String(topic);

    char payloadCharAr[length];

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
        payloadCharAr[i] = (char)payload[i];
    }
    Serial.println(" ");

    if (topics == "MEDIBOX-NANDU-MINANGLE")
    {
        Serial.print("Min Angle Updated - ");
        minAngle = atoi(payloadCharAr);
        Serial.println(minAngle);
        Serial.println();
        delay(2000);
    }
    else if (topics == "MEDIBOX-NANDU-CONTFACTOR")
    {
        Serial.print("Control Factor Updated - ");
        controlFactor = atof(payloadCharAr);
        Serial.println(controlFactor);
        Serial.println();
        delay(2000);
    }
 
 }

void connectToBroker(){
  while(!mqttClient.connected()){
    Serial.print("Attempting MQTT connection...");
    if(mqttClient.connect("ESP32-1234545245343")){
      mqttClient.subscribe("MEDIBOX-NANDU-MINANGLE");
      mqttClient.subscribe("MEDIBOX-NANDU-CONTFACTOR");
    }else{
      Serial.print("failed ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void updateTemperature(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  String(data.temperature, 2).toCharArray(tempAr, 6);
}

void setupWifi(){
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print("Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
