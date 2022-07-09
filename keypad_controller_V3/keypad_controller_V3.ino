#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include "DHT.h"
#define rx_tx_pin               2
#define DHTPIN 3  
#define DHTTYPE DHT22


#define firmware_version  "V3.0.0"


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 102);
IPAddress server(192, 168, 0, 105);
//EthernetServer server(80);

int adopted_eeprom_addr = 0;
int uuid_eeprom_addr = 2;
int alias_eeprom_addr = 4;
int controller_uuid;
char char_controller_uuid[4];
char* controller_alias;
int device_state = 0;   //0 = booting, 1 = pending adoption, 2 = running

const char* mqtt_device_topic = "L_S_C/";
const char* mqtt_adoption_topic = "/adoption";
const char* mqtt_stats_topic = "/stats";
const char* mqtt_state_topic = "/state";
const char* mqtt_ping_topic = "/ping";
const char* mqtt_sync_topic = "/sync";


byte online =     1;
byte offline =    0;
byte device_list[][3] =   //id 
                          //device type 0 = unasigned, 1 = keypad
{
  0,  0,  offline,
  1,  0,  offline,
  2,  0,  offline,
  3,  0,  offline,

};

bool sync_button = false;
bool ping_pending = false;

void new_device() {
  int i = 1;
  ping_devices();
 
  while(i < 17){                                                              //////////////////
      if(device_list[i][2] == offline){
        int id = i;
        Serial.print('I');
        Serial.print('N');
        Serial.print('P');
        Serial.print(id);
        Serial.print('F');
        break;
        }
      ping_devices();
  }
  
}

void sync() {
  digitalWrite(rx_tx_pin, HIGH);
  Serial.print('I'); //initiate data packet
  Serial.print('Z'); //code for unassigned
  Serial.print('S'); //code for Sync
  Serial.print('F'); //finish data packet
  Serial.flush();    
  
  digitalWrite(rx_tx_pin, LOW);
  
  sync_button = false;
  return;
  
  if(Serial.find('i')) {
      if(Serial.read() =='z') {
          if(Serial.read() =='n') {
              if(Serial.read() =='n') {
                  if(Serial.read()=='f') //finish reading
                      {
                
                         digitalWrite(rx_tx_pin, HIGH);
                         new_device(); 
                      }
              } 
          }
      
      }

  }
  sync_button = false;
}

void ping_devices(){
  
  
  for (int i = 1; i<5; i++){
  //while (i < 17) {                      //////////
    digitalWrite(rx_tx_pin, HIGH);
    Serial.print('I');
    Serial.print(i);
    Serial.print('P');
    Serial.flush();  
    digitalWrite(rx_tx_pin, LOW);
    delay(50); 
    if(Serial.find('i')) {
        
        int id = Serial.parseInt();
        
        if(Serial.read() == 'p'){
          int device_type=Serial.parseInt(); 
          if(Serial.read()=='f') //finish reading
          {
            digitalWrite(5, HIGH);
            device_list[i][1] = device_type;
            device_list[i][2] = online;
            delay(100);
            digitalWrite(5, LOW);
          }
         
       }       
     }
     else{
          device_list[i][2] = offline;
     }
    
    
  }
  digitalWrite(rx_tx_pin, LOW);
  Serial.println();
  ping_pending = true;
  
}




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


  if (device_state == 1) {    //if in pending adoption state
      StaticJsonDocument<20> adoption;      //create doc
      DeserializationError error = deserializeJson(adoption, payload); 
      
      // Test if parsing succeeds.
     // if (error) {
      //  Serial.print(F("deserializeJson() failed: "));
       // Serial.println(error.f_str());
       // return;
      //}
    
      controller_alias = adoption["alias"];
      int int_controller_uuid = adoption["uuid"];
      dtostrf(int_controller_uuid, 3,0,controller_uuid);//convert to char with 3 digits
      EEPROM.write(uuid_eeprom_addr, controller_uuid);
      EEPROM.put(alias_eeprom_addr, controller_alias);
     // Serial.println(controller_alias);
      Serial.println(controller_uuid);
      EEPROM.write(adopted_eeprom_addr, 102);//successfully adopted
      device_state = 2;   //set state to running
      
      }
  //}
  Serial.println();
  if(device_state == 2){
    char test_topic[20];
    strcpy(test_topic, mqtt_device_topic);
    dtostrf(controller_uuid, 2,0,char_controller_uuid);
    strcat(test_topic, char_controller_uuid);
    strcat(test_topic, mqtt_ping_topic);
   // sprintf(test_topic, "%s%d%s", mqtt_device_topic,controller_uuid,mqtt_ping_topic);
    
    if (strcmp(topic,test_topic)==0){
      if (strcmp(str,"ping")==0){
        //Serial.print("pinging");
        ping_devices();
        }
    }
    //sprintf(topic, "%s%d%s", mqtt_device_topic,controller_uuid,mqtt_sync_topic);
    strcpy(test_topic, mqtt_device_topic);
    dtostrf(controller_uuid, 2,0,char_controller_uuid);
    strcat(test_topic, char_controller_uuid);
    strcat(test_topic, mqtt_sync_topic);
    if (strcmp(topic,test_topic)==0){
      if (strcmp(str,"sync")==0){
        Serial.print("should sync");
        sync_button = true;
      }
    }
  }
  }


EthernetClient ethClient;
PubSubClient client(ethClient);
DHT dht(DHTPIN, DHTTYPE);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("keypad_controller_v3")) {
      Serial.println("connected");
      // Once connected, publish connected
      
      char topic[38];
      strcpy(topic, mqtt_device_topic);
      dtostrf(controller_uuid, 2,0,char_controller_uuid);
      strcat(topic, char_controller_uuid);
      strcat(topic, mqtt_stats_topic);
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
  
   if(adopted == 102){//arpitary value that shows has already been adopted
      //has already been adopted
      
      controller_uuid = EEPROM.read(uuid_eeprom_addr);
      //dtostrf(controller_uuid, 3,0,controller_uuid);//convert to char with 3 digits
      Serial.print("Controller ID: ");
      Serial.println(controller_uuid);
      //controller_alias = EEPROM.get(alias_eeprom_addr);
      //Serial.print("controller alias: TODO");
      //Serial.println(controller_alias);
     
      reconnect();
      client.loop();
      device_state = 2;
      char topic[20];
      sprintf(topic, "%s%d%s", mqtt_device_topic,controller_uuid,mqtt_ping_topic);
      client.subscribe(topic);
      sprintf(topic, "%s%d%s", mqtt_device_topic,controller_uuid,mqtt_sync_topic);
      client.subscribe(topic);
     }
   else{
      
      reconnect();
      client.loop();
      delay(1000);
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


void press_button(int id, int button_id){
  //Serial.println(id);

  //Serial.print(button_id);
  char topic[30];
  sprintf(topic, "%s%d/devices/%d/buttons/%d", mqtt_device_topic,controller_uuid, id, button_id);
  
  client.publish(topic, "pressed");
  
}  
void release_button(int id, int button_id){
  //Serial.println(id);

  //Serial.print(button_id);
  char topic[30];
  sprintf(topic, "%s%d/devices/%d/buttons/%d", mqtt_device_topic,controller_uuid, id, button_id);
  
  client.publish(topic, "released");
}  


void setup() {
   //initialise serial
   Serial.begin(9600);
   //setup rs485 com pin
   pinMode(rx_tx_pin, OUTPUT);
   digitalWrite (rx_tx_pin, LOW );
   pinMode(5, OUTPUT);
   //start temp sensor
   dht.begin();
   //setup mqtt server
   client.setServer(server, 1883);
   client.setCallback(callback);
   Ethernet.begin(mac, ip);
   // Allow the hardware to sort itself out
   delay(1500);
   Serial.print("L_S_C started @ ");
   Serial.println(ip);
   Serial.print("Firmware: ");
   Serial.println(firmware_version);
   get_controller_id();//this will get controller id before starting mqtt server
   ping_devices();
   
   
}

void loop() {
  
  client.loop();
  if (!client.connected()) {  reconnect();    }

  if (sync_button == true) {
    Serial.println("syncing");
    sync();
  }

  if(ping_pending){
    char topic[20];
    for(int device_id=1; device_id<5; device_id++){
    sprintf(topic, "%s%d/ping/device_%d/type", mqtt_device_topic,controller_uuid, device_id);
    if((device_list[device_id][1]) == 0){client.publish(topic,"unasigned");}
    if((device_list[device_id][1]) == 1){client.publish(topic,"keypad");}
    
    sprintf(topic, "%s%d/ping/device_%d/state", mqtt_device_topic,controller_uuid, device_id);
    if((device_list[device_id][2]) == 0){client.publish(topic,"offline");}
    if((device_list[device_id][2]) == 1){client.publish(topic,"online");}

    }
    ping_pending = false;
  }

  
  digitalWrite(rx_tx_pin, LOW);
  if ( Serial.available ()) {
      if ( Serial.read () == 'i' ) {
        int device_id = Serial.parseInt ();
        
        
        char function = Serial.read ();
        if (function == 'p' ) {
            int button_id = Serial.parseInt ();
            
            if ( Serial.read () == 'f' ) {
                      press_button(device_id,button_id);
                      Serial.print("Button ");
                      Serial.print(button_id);
                      Serial.println(" pressed.");
                      }
                    }
                
         else if (function == 'r' ) {
            int button_id = Serial.parseInt ();
            
            if ( Serial.read () == 'f' ) {
                      release_button(device_id,button_id);
                      Serial.print("Button ");
                      Serial.print(button_id);
                      Serial.println(" released.");
                      }
                    }
                
        
}
}
}
