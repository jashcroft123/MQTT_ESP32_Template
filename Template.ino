#include <WiFi.h>
#include <PubSubClient.h>

/*****************  START USER CONFIG SECTION *********************************/
/*****************  START USER CONFIG SECTION *********************************/
/*****************  START USER CONFIG SECTION *********************************/
/*****************  START USER CONFIG SECTION *********************************/

#define USER_SSID                 "****"
#define USER_PASSWORD             "****"
#define USER_MQTT_SERVER          "****"
#define USER_MQTT_PORT            1883
#define USER_MQTT_USERNAME        "****"
#define USER_MQTT_PASSWORD        "****"
#define USER_MQTT_CLIENT_NAME     "Bigroom/BME280"         // Used to define MQTT topics, MQTT Client ID, and ArduinoOTA

/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/


// Replace the next variables with your SSID/Password combination
const char* ssid = USER_SSID;
const char* password = USER_PASSWORD;
const char* mqtt_server = USER_MQTT_SERVER;
const char *mqtt_user = USER_MQTT_USERNAME;
const char *mqtt_pass = USER_MQTT_PASSWORD;
const char *mqtt_client_name = USER_MQTT_CLIENT_NAME ; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
bool boot = true;


void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  int retries = 0;
  while (!client.connected()) {
    if(retries < 150)
    {
      Serial.print("Attempting MQTT connection...");
      if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass)) 
      {
        Serial.println("connected");
        if(boot == false)
        {
          client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Reconnected"); 
        }
        if(boot == true)
        {
          client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Rebooted");
        }
        // ... and resubscribe
        client.subscribe(USER_MQTT_CLIENT_NAME"/command");
        client.subscribe(USER_MQTT_CLIENT_NAME"/positionCommand");
      } 
      else 
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        retries++;
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    if(retries > 149)
    {
    ESP.restart();
    }
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
