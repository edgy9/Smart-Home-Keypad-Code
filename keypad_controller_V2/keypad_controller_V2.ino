#include <Ethernet.h>
#include <MQTT.h>
#include "DHT.h"

#define DHTPIN 2  
#define DHTTYPE DHT22
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 1, 102};  // <- change to match your network

bool sync_button = false;
unsigned long lastMillis = 0;
const int controller_id = 1
int online =     1;
int offline =    0;
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

EthernetClient net;
MQTTClient client;
DHT dht(DHTPIN, DHTTYPE);

void connect() {
  Serial.print("connecting...");
  while (!client.connect("arduino", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
  char path = "/light_switch_controller/" + controller_id
  client.subscribe(path);
  // client.unsubscribe("/hello");
}


void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}


void ping_devices(){
  digitalWrite(2, HIGH);
  int i = 1;
  while (i < 17) {
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

void update_button(int id, int button_id){
  Serial.println(id, button_id);
}
void setup() {
   Serial.begin(9600);
   Ethernet.begin(mac, ip);
   pinMode(2, OUTPUT);
   digitalWrite (2, HIGH );
   dht.begin();
   client.begin("192.168.1.25", net);
   client.onMessage(messageReceived);
   connect();
   ping_devices();
   
}
void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  if (millis() - lastMillis > 10000) {       //send temperature every 10 seconds
    lastMillis = millis();
    char h_path = "/light_switch_controller/" + controller_id + "/stats/humidity/";
    char t_path = "/light_switch_controller/" + controller_id + "/stats/temperature"/;
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    client.publish(h_path, temperature);
    
  if (sync_button == true) {
    sync();
  }
   if ( Serial.available ()) {
        if ( Serial.read () == 'i' ) {
          char id = Serial.read();
          char function = Serial.read ();
          if (function == 'u' ) {
              char button_id = Serial.read ();
              if ( Serial.read () == 'f' ) {
                        update_button(id,button_id);
                        }
                      }
                  }

}

  
}

void new_device() {
  int i = 1;
  ping_devices();
  while(i < 17){
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
