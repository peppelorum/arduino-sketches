/*  ___   ___  ___  _   _  ___   ___   ____ ___  ____  
 * / _ \ /___)/ _ \| | | |/ _ \ / _ \ / ___) _ \|    \ 
 *| |_| |___ | |_| | |_| | |_| | |_| ( (__| |_| | | | |
 * \___/(___/ \___/ \__  |\___/ \___(_)____)___/|_|_|_|
 *                  (____/ 
 * Tutorial URL http://osoyoo.com/2016/11/24/use-nodemcu-to-send-temperaturehumidity-data-to-mqtt-iot-broker/
 * Created by John Yu
 * Modified by Peppe Hämenniemi
 * 
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "wifi";
const char* password = "pw";

const char* mqtt_server = "server";
int mqtt_server_port = 18772;
const char* mqtt_username = "user";
const char* mqtt_password = "pw";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Water level is: ");
  Serial.print(topic);
  int p =(char)payload[0]-'0';
  int chk = analogRead(A0);

  Serial.println();
} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect 
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password))
    //if (client.connect(clientId.c_str())) //Row for MQTT broker without username
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("granen");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_server_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  // read sensor every 6 seconds
  if (now - lastMsg > 6000) {
     lastMsg = now;
     String msg= String(analogRead(A0));
     char message[10];
     msg.toCharArray(message,10);
     Serial.println(message);
     //publish sensor data to MQTT broker
    client.publish("granen", message);
  }
}
