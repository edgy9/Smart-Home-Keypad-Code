#include <Ethernet.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 3  
#define DHTTYPE DHT22
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 102);
IPAddress server(192, 168, 1, 25);

bool sync_button = false;
unsigned long lastMillis = 0;
const int controller_id = 1;
byte online =     1;
byte offline =    0;
byte device_list[][3] =   //id 
                          //device type 0 = unasigned, 1 = keypad
{
  0,  0,  offline,
  1,  0,  offline,
  2,  0,  offline,
  3,  0,  offline,
  4,  0,  offline,
  5,  0,  offline,
  6,  0,  offline,
  7,  0,  offline,
  8,  0,  offline,
  9,  0,  offline,
  10, 0,  offline,
  11, 0,  offline,
  12, 0,  offline,
  13, 0,  offline,
  14, 0,  offline,
  15, 0,  offline,
};
const char* mqtt_device_topic = "L_S_C/1";
const char* h_topic = "/stats/H/";
const char* t_topic = "/stats/T/";
const char* mac_topic = "/stats/mac/";
const char* ip_topic = "/stats/ip/";
const char* state_topic = "/stats/state/";



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


EthernetClient ethClient;
PubSubClient client(ethClient);
DHT dht(DHTPIN, DHTTYPE);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      char topic[38];
      strcpy(topic, mqtt_device_topic);
      strcat(topic, state_topic);
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




    
  

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}


void ping_devices(){
  digitalWrite(2, HIGH);
  int i = 1;
  while (i < 2) {
  //while (i < 17) {                      //////////
    i = i + 1;
    Serial.print("I");
    Serial.print(i);
    Serial.print("P");
    
    digitalWrite(2, LOW);
    
    if(Serial.find("i")) {
        if(Serial.read() == i) {
          if(Serial.read() == "p"){
          int device_type=Serial.parseInt(); 
            if(Serial.read()=='f') //finish reading
            {
              device_list[i][1] = device_type;
              device_list[i][2] = online;
            }
          } 
         }       
     }
     else{
          device_list[i][2] = offline;
     }
    
    
  }
}

void send_start_stats(){
    
    Serial.print("sending start stats");



    
    float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  Serial.print(f);
  Serial.print(t);
  Serial.print(h);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }


  


  
    float f_temperature = dht.readTemperature();
    char temperature[5];
    dtostrf(f_temperature, 5, 2, temperature); 
    Serial.print(temperature);
    
    float f_humidity = dht.readHumidity();
    char humidity[5];
    dtostrf(f_humidity, 5, 2, humidity); 
    Serial.print(f_humidity);
    
    char topic[50];
    strcpy(topic, mqtt_device_topic);
    strcat(topic, t_topic);
    client.publish(topic, temperature);
    
    strcpy(topic, mqtt_device_topic);
    strcat(topic, h_topic);
    client.publish(topic, humidity);
    
    strcpy(topic, mqtt_device_topic);
    strcat(topic, mac_topic);
    client.publish(topic, mac);
    
    strcpy(topic, mqtt_device_topic);
    strcat(topic, ip_topic);
    client.publish(topic, ip);
    
    

  
}

void update_button(int id, int button_id){
  Serial.println(id, button_id);
  char topic[30]
  strcpy(topic, mqtt_device_topic);
  strcat(topic, "/devices");
  strcat(topic, id);
  strcat(topic, "/");
  strcat(topic, button_id);
  client.publish(topic, "pressed");
  delay(10);
  client.publish(topic, "released");
}
void setup() {
   Serial.begin(9600);
   
   pinMode(2, OUTPUT);
   digitalWrite (2, HIGH );
   dht.begin();
   client.setServer(server, 1883);
   client.setCallback(callback);
   Ethernet.begin(mac, ip);
   // Allow the hardware to sort itself out
   delay(1500);
   //ping_devices();
   //send_start_stats();
}
void send_stats(){
    lastMillis = millis();

    char topic[50];
    strcpy(topic, mqtt_device_topic);
    strcat(topic, t_topic);
    
    float f_temperature = dht.readTemperature();
    char temperature[5];
    dtostrf(f_temperature, 5, 2, temperature); 
    //Serial.write(temperature);
    
    
    client.publish(topic, temperature);
    
    float f_humidity = dht.readHumidity();
    char humidity[5];
    dtostrf(f_humidity, 5, 2, humidity); 
    
    //Serial.println(humidity);
    
    
       
    strcpy(topic, mqtt_device_topic);
    strcat(topic, h_topic);
    client.publish(topic, humidity);
    //Serial.println(humidity);
    
    //client.publish(t_topic, temperature);
   // client.publish(h_topic, humiditity);
}
void loop() {
  
  client.loop();
  if (!client.connected()) {  reconnect();    }
  if (millis() - lastMillis > 10000) {    send_stats();    }   //send temperature every 10 seconds
    
   
  if (sync_button == true) {
    sync();
  }

  
  if ( Serial.available ()) {
      if ( Serial.read () == 'i' ) {
        char device_id = Serial.read();
        char function = Serial.read ();
        if (function == 'u' ) {
            char button_id = Serial.read ();
            if ( Serial.read () == 'f' ) {
                      update_button(device_id,button_id);
                      }
                    }
                }

}

  
}

void new_device() {
  int i = 1;
  ping_devices();
 
  while(i < 17){                                                              //////////////////
      if(device_list[i][2] == offline){
        int id = i;
        Serial.print("I");
        Serial.print("N");
        Serial.print("P");
        Serial.print(id);
        Serial.print("F");
        break;
        }
      ping_devices();
  }
  
}

void sync() {
  
  Serial.print("I"); //initiate data packet
  Serial.print("Z"); //code for unassigned
  Serial.print("S"); //code for Sync
  Serial.print("F"); //finish data packet
  Serial.flush();    
  
  digitalWrite(2, LOW);
  
  if(Serial.find("i")) {
      if(Serial.read() =='z') {
          if(Serial.read() =='n') {
              if(Serial.read() =='n') {
                  if(Serial.read()=='f') //finish reading
                      {
                
                         digitalWrite(2, HIGH);
                         new_device(); 
                      }
              } 
          }
      
      }

  }
}
