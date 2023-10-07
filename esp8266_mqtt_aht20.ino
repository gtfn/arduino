/* Modifiziertes Beispiel vom ESP8266 Sketch von
   https://diyi0t.com/microcontroller-to-raspberry-pi-wifi-mqtt-communication/
   und dem AHT20 Beispiel von Qwiic
*/
#include <Wire.h>
#include "PubSubClient.h" // Connect and publish to the MQTT broker
#include <SparkFun_Qwiic_Humidity_AHT20.h>
// http://librarymanager/All#Qwiic_Humidity_AHT20 by SparkFun
#include "ESP8266WiFi.h"  // Enables the ESP8266 WiFi

AHT20 humiditySensor;


// WiFi
const char* ssid = "WLAN-C4XQ6B";               // network SSID
const char* wifi_password = "8693608051866256"; // network password

// MQTT
const char* mqtt_server = "192.168.2.103";  // IP of the MQTT broker
const char* humidity_topic = "haus/EG/grWohnz/humidity";
const char* temperature_topic = "haus/EG/grWohnz/temperature";
const char* mqtt_username = "frank"; // MQTT username
const char* mqtt_password = "frank"; // MQTT password
const char* clientID = "gr_wohnzimmer"; // MQTT client ID

// Konstanten
const long intervallSerial = 60000; // Serial.print 1 min.
const long intervallMQTT = 300000; // publish_MQTT alle 5 min.

// Variablen
unsigned long prevMillis[] = {0, 0}; // speichere letzte zeit
float temperature = 0;
float humidity = 0;

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient);

// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection failed...");
  }
}

void publish_MQTT(const char* the_topic, float the_value) {
  // PUBLISH to the MQTT Broker
  if (client.publish(the_topic, String(the_value).c_str())) {
    Serial.println("Temperature sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Temp. failed to send. Reconnect and try again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(the_topic, String(the_value).c_str());
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("ESP8266 AHT20 Sensor");

  Wire.begin(); //Join I2C bus

  //Check if the AHT20 will acknowledge
  if (humiditySensor.begin() == false)
  {
    Serial.println("AHT20 not detected. Freezing.");
    while (1);
  }
  Serial.println("AHT20 OK.");
}

void loop() {
  unsigned long jetztMillis = millis();
  if (jetztMillis - prevMillis[0] >= intervallSerial)
  {
    prevMillis[0] = jetztMillis;
    //Get the new temperature and humidity value
    temperature = humiditySensor.getTemperature();
    humidity = humiditySensor.getHumidity();

    // check if returns are valid and print the sensor data
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from AHT");
    } else {
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" *C");
    }
  }
  jetztMillis = millis();
  if (jetztMillis - prevMillis[1] >= intervallMQTT)
  {
    prevMillis[1] = jetztMillis;
    connect_MQTT();
    Serial.setTimeout(2000);
    publish_MQTT(temperature_topic, temperature);
    publish_MQTT(humidity_topic, humidity);
    // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
    client.disconnect();  // disconnect from the MQTT broker
    //delay(1000 * 60);     // print new values every 1 Minute
  }
}
