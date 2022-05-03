#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include "DHT.h"
#define rx_tx_pin               2
#define DHTPIN 3  
#define DHTTYPE DHT22
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 102);
IPAddress server(192, 168, 0, 105);
//EthernetServer server(80);

int adopted_eeprom_addr = 0;
int uuid_eeprom_addr = 2;
int alias_eeprom_addr = 4;
int device_uuid;
char* device_alias;
int device_state = 0;   //0 = booting, 1 = pending adoption, 2 = running

const char* mqtt_device_topic = "L_S_C";
const char* mqtt_adoption_topic = "/adoption";
const char* mqtt_state_topic = "/state";

void callback(char* topic, byte* payload, unsigned int length) {
  char str[length+1];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  int i =0;
  for (i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    str[i]=(char)payload[i];
  }
  
  str[i] = 0; // Null termination
  Serial.println();


  if (device_state == 1) {    /.if in pending adoption state
      StaticJsonDocument<20> adoption;      //create doc
      DeserializationError error = deserializeJson(adoption, payload); 
      
      // Test if parsing succeeds.
     // if (error) {
      //  Serial.print(F("deserializeJson() failed: "));
       // Serial.println(error.f_str());
       // return;
      //}
    
      device_alias = adoption["alias"];
      device_uuid = adoption["uuid"];
      EEPROM.write(uuid_eeprom_addr, device_uuid);
      EEPROM.put(alias_eeprom_addr, device_alias);
      Serial.println(device_alias);
      Serial.println(device_uuid);
      EEPROM.write(adopted_eeprom_addr, 100);//successfully adopted
      device_state = 2;   //set state to running
      }
  //}
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);
DHT dht(DHTPIN, DHTTYPE);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      char topic[38];
      strcpy(topic, mqtt_device_topic);
      strcat(topic, mqtt_state_topic);
      client.publish(topic,"Connected");
      // ... and resubscribe
      
      client.subscribe(topic);
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void get_controller_id(){
   int adopted = 0;
   adopted = EEPROM.read(adopted_eeprom_addr);
   Serial.print(adopted);
   if(adopted == 100){
      //has already been adopted
      int id = 0;
      id = EEPROM.read(uuid_eeprom_addr);
      Serial.print("controller id:");
      Serial.println(id);
     }
   else{
      Serial.print("not adopted");
      char topic[50];
      strcpy(topic, mqtt_device_topic);
      strcat(topic, mqtt_adoption_topic);
      Serial.println(topic);
      client.publish(topic, "pending_adoption");

      client.subscribe(topic);
      device_state = 1;
   }
}
  
void setup() {
   //initialise serial
   Serial.begin(9600);
   //setup rs485 com pin
   pinMode(rx_tx_pin, OUTPUT);
   digitalWrite (rx_tx_pin, LOW );
   //start temp sensor
   dht.begin();
   //setup mqtt server
   client.setServer(server, 1883);
   client.setCallback(callback);
   Ethernet.begin(mac, ip);
   // Allow the hardware to sort itself out
   delay(1500);
   Serial.print("started");
   Serial.println(ip);
   //start mqtt server
   //server.begin();
   reconnect();
   client.loop();
   get_controller_id();
}

void loop() {
  
  client.loop();
  if (!client.connected()) {  reconnect();    }
}
