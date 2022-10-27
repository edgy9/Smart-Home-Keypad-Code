

#include "defines.h"
#include <stdio.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

//#include "DHT.h"
#define rx_tx_pin               4
//#define DHTPIN 3  
//#define DHTTYPE DHT22

#define EEPROM_SIZE 2
#define firmware_version  "V3.0.0"

EthernetClient ethClient;

//byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
//IPAddress ip(192, 168, 0, 102);
IPAddress server(192, 168, 0, 105);
//EthernetServer server(80);

int adopted_eeprom_addr = 0;
int uuid_eeprom_addr = 1;
int alias_eeprom_addr = 2;
uint8_t controller_uuid;
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
byte device_list[4][3] =   //id 
                          //device type 0 = unasigned, 1 = keypad
{
  0,  0,  offline,
  1,  0,  offline,
  2,  0,  offline,
  3,  0,  offline

};

bool sync_button = false;
bool ping_pending = false;


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
        
        //int id = Serial.parseInt();
        Serial.parseInt();
        
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
          device_list[i][1] = 0;
     }
    
    
  }
  digitalWrite(rx_tx_pin, LOW);
  Serial.println();
  ping_pending = true;
  
}
void new_device() {
  int i = 1;
  ping_devices();
 
  while(i < 5){                                                              //////////////////
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
  Serial.print('Z'); //initiate data packet
  Serial.print('I'); //code for unassigned
  Serial.print('S'); //code for Sync
  Serial.print('F'); //finish data packet
  Serial.flush();    
  
  digitalWrite(rx_tx_pin, LOW);
  
  sync_button = false;
  /*
  
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

  }*/
  sync_button = false;
}






void callback(char* topic, byte* payload, unsigned int length) {
  digitalWrite(rx_tx_pin, LOW);
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
      Serial.println("about to create json doc");
      StaticJsonDocument<20> adoption;      //create doc
      DeserializationError error = deserializeJson(adoption, payload); 
      
      // Test if parsing succeeds.
     if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
    
      //controller_alias = adoption["alias"];
      const uint8_t int_controller_uuid = adoption["uuid"];
      Serial.println(int_controller_uuid);
      //dtostrf(int_controller_uuid, 3,0,controller_uuid);//convert to char with 3 digits
      EEPROM.write(uuid_eeprom_addr, int_controller_uuid);
      EEPROM.commit();
      //EEPROM.put(alias_eeprom_addr, controller_alias);
     // Serial.println(controller_alias);
      //Serial.println(controller_uuid);
      EEPROM.write(adopted_eeprom_addr, 102);//successfully adopted
      EEPROM.commit();
      Serial.println("successfully adopted");
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


//EthernetClient ethClient;
PubSubClient client(ethClient);
//DHT dht(DHTPIN, DHTTYPE);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("keypad_controller_v3")) {
      Serial.println("connected");
      // Once connected, publish connected
      
      char topic[30];
      snprintf(topic, 30, "%s%d%s%s", mqtt_device_topic, controller_uuid, mqtt_stats_topic, mqtt_state_topic);
      
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
   Serial.print("adopted?");
   Serial.println(adopted);
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
   Serial.begin(115200);
   while (!Serial && millis() < 5000);


  //initialize eeprom
   Serial.println("starting EEPROM");
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
  }

  
   //setup rs485 com pin
   pinMode(rx_tx_pin, OUTPUT);
   digitalWrite (rx_tx_pin, LOW );
   
   //start temp sensor
   //dht.begin();
   Serial.print("\nStarting Controller on "); Serial.print(BOARD_NAME);
   Serial.print(F(" with ")); Serial.println(SHIELD_TYPE); 
   Serial.println(ETHERNET_GENERIC_VERSION);

   #if (USING_SPI2)
  #if defined(CUR_PIN_MISO)
    ETG_LOGWARN(F("Default SPI pinout:"));
    ETG_LOGWARN1(F("MOSI:"), CUR_PIN_MOSI);
    ETG_LOGWARN1(F("MISO:"), CUR_PIN_MISO);
    ETG_LOGWARN1(F("SCK:"),  CUR_PIN_SCK);
    ETG_LOGWARN1(F("SS:"),   CUR_PIN_SS);
    ETG_LOGWARN(F("========================="));
  #endif
#else
  ETG_LOGWARN(F("Default SPI pinout:"));
  ETG_LOGWARN1(F("MOSI:"), MOSI);
  ETG_LOGWARN1(F("MISO:"), MISO);
  ETG_LOGWARN1(F("SCK:"),  SCK);
  ETG_LOGWARN1(F("SS:"),   SS);
  ETG_LOGWARN(F("========================="));
#endif

#if defined(ESP8266)
  // For ESP8266, change for other boards if necessary
  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   D2    // For ESP8266
  #endif

  ETG_LOGWARN1(F("ESP8266 setCsPin:"), USE_THIS_SS_PIN);

  // For ESP8266
  // Pin                D0(GPIO16)    D1(GPIO5)    D2(GPIO4)    D3(GPIO0)    D4(GPIO2)    D8
  // Ethernet_Generic   X                 X            X            X            X        0
  // D2 is safe to used for Ethernet_Generic libs
  //Ethernet.setCsPin (USE_THIS_SS_PIN);
  Ethernet.init (USE_THIS_SS_PIN);

#elif defined(ESP32)

  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  Ethernet.init(26);  // ESP32 with Adafruit Featherwing Ethernet

  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   5  //22    // For ESP32
  #endif

  ETG_LOGWARN1(F("ESP32 setCsPin:"), USE_THIS_SS_PIN);

  // Must use library patch for Ethernet, EthernetLarge libraries
  // ESP32 => GPIO2,4,5,13,15,21,22 OK with Ethernet, Ethernet2, EthernetLarge
  // ESP32 => GPIO2,4,5,15,21,22 OK with Ethernet3

  //Ethernet.setCsPin (USE_THIS_SS_PIN);
  Ethernet.init (USE_THIS_SS_PIN);
  
#elif ETHERNET_USE_RPIPICO

  pinMode(USE_THIS_SS_PIN, OUTPUT);
  digitalWrite(USE_THIS_SS_PIN, HIGH);
  
  // ETHERNET_USE_RPIPICO, use default SS = 5 or 17
  #ifndef USE_THIS_SS_PIN
    #if defined(ARDUINO_ARCH_MBED)
      #define USE_THIS_SS_PIN   17     // For Arduino Mbed core
    #else  
      #define USE_THIS_SS_PIN   17    // For E.Philhower core
    #endif
  #endif

  ETG_LOGWARN1(F("RPIPICO setCsPin:"), USE_THIS_SS_PIN);

  // Must use library patch for Ethernet, EthernetLarge libraries
  // For RPI Pico using Arduino Mbed RP2040 core
  // SCK: GPIO2,  MOSI: GPIO3, MISO: GPIO4, SS/CS: GPIO5
  // For RPI Pico using E. Philhower RP2040 core
  // SCK: GPIO18,  MOSI: GPIO19, MISO: GPIO16, SS/CS: GPIO17
  // Default pin 5/17 to SS/CS

  //Ethernet.setCsPin (USE_THIS_SS_PIN);
  Ethernet.init (USE_THIS_SS_PIN);
  
#else   //defined(ESP8266)
  // unknown board, do nothing, use default SS = 10
  #ifndef USE_THIS_SS_PIN
    #define USE_THIS_SS_PIN   10    // For other boards
  #endif

  #if defined(BOARD_NAME)
    ETG_LOGWARN3(F("Board :"), BOARD_NAME, F(", setCsPin:"), USE_THIS_SS_PIN);
  #else
    ETG_LOGWARN1(F("Unknown board setCsPin:"), USE_THIS_SS_PIN);
  #endif

  // For other boards, to change if necessary 
  Ethernet.init (USE_THIS_SS_PIN);

#endif    // defined(ESP8266)

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;
  // Use Static IP
  //Ethernet.begin(mac[index], ip);
  Ethernet.begin(mac[index]);

  //SPIClass SPI2(HSPI);
  //Ethernet.begin(mac[index], &SPI2);

  // Just info to know how to connect correctly
  // To change for other SPI
#if defined(CUR_PIN_MISO)
  ETG_LOGWARN(F("Currently Used SPI pinout:"));
  ETG_LOGWARN1(F("MOSI:"), CUR_PIN_MOSI);
  ETG_LOGWARN1(F("MISO:"), CUR_PIN_MISO);
  ETG_LOGWARN1(F("SCK:"),  CUR_PIN_SCK);
  ETG_LOGWARN1(F("SS:"),   CUR_PIN_SS);
  ETG_LOGWARN(F("========================="));
#else
  ETG_LOGWARN(F("Currently Used SPI pinout:"));
  ETG_LOGWARN1(F("MOSI:"), MOSI);
  ETG_LOGWARN1(F("MISO:"), MISO);
  ETG_LOGWARN1(F("SCK:"),  SCK);
  ETG_LOGWARN1(F("SS:"),   SS);
  ETG_LOGWARN(F("========================="));
#endif

  Serial.print(F("Using mac index = "));
  Serial.println(index);

  Serial.print(F("Connected! IP address: "));
  Serial.println(Ethernet.localIP());

  if ( (Ethernet.getChip() == w5500) || (Ethernet.getAltChip() == w5100s) )
  {
    Serial.print(F("Speed: "));    Serial.print(Ethernet.speedReport());
    Serial.print(F(", Duplex: ")); Serial.print(Ethernet.duplexReport());
    Serial.print(F(", Link status: ")); Serial.println(Ethernet.linkReport());
  }

  // start the web server on port 80
  //server.begin();



   //setup mqtt server
   client.setServer(server, 1883);
   client.setCallback(callback);
   //Ethernet.begin(mac, ip);
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
    Serial.println("Ping pending");
    char topic[30];
    
    //for (int device_id=1; device_id <= 4; device_id++){
    int device_id;
    for (int i=0; i <= 3; i++){
      //Serial.println(mqtt_device_topic);
      //Serial.println(controller_uuid);
      Serial.println(i);
      device_id = i;
      Serial.println(device_id);
     
      snprintf(topic, 30, "%s%d/ping/device_%d/type", mqtt_device_topic, controller_uuid, device_id);
      
      if((device_list[device_id][1]) == 0){client.publish(topic,"unasigned");}
      if((device_list[device_id][1]) == 1){client.publish(topic,"keypad");}
      
      snprintf(topic, 30, "%s%d/ping/device_%d/state", mqtt_device_topic,controller_uuid, device_id);
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
