#include <API.h>

#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#include <aWOT.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetServer server(80);
//Application app;
//EthernetClient client
API api();


void setup() {
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial) continue;

  // Initialize Ethernet libary
  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize Ethernet library"));
    return;
  }
  
  //server.begin();
  api.begine();
  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());
}
/*
void json_device_button(JsonVariant button_state) {
  JsonObject buttonState = button_state.createNestedObject("button_state");
  buttonState["1"] = analogRead(1);
  
  buttonState["2"] = analogRead(2);
  
  buttonState["3"] = analogRead(3);
  
  buttonState["4"] = analogRead(4);
  
  buttonState["5"] = analogRead(5);
  
  buttonState["6"] = analogRead(1);
  
  buttonState["7"] = analogRead(2);
}

void create_slave_json(JsonVariant slaves){
  for (int i=0; i<10; i++){
    char slave_id [3];
    sprintf(slave_id, "slave_%d", i);
    JsonObject slaves = slaves.createNestedObject(slave_id);
  }
  
}
void create_config_json(){
  
  DynamicJsonDocument config_doc(500);
  
  JsonObject masterInfo = doc.createNestedObject("master_info");
  JsonArray device_ip = masterInfo.createNestedArray("ip");
  device_ip.add(Ethernet.localIP());
  
  JsonArray device_mac = masterInfo.createNestedArray("mac");
  device_mac.add(mac);
  
  JsonArray device_humidity = masterInfo.createNestedArray("humidity");
  float h = dht.readHumidity();
  device_humidity.add(h)
  
  JsonArray device_temperature = masterInfo.createNestedArray("temperature");
  float t = dht.readTemperature();
  device_temperature.add(t);

  
}
void create_state_json(){
  DynamicJsonDocument state_doc(500);
  JsonObject slavesConnected = doc.createNestedObject("slave_devices");
  JsonVariant slaves = doc["slave_devices"];
  create_slave_json(slaves);

  
  JsonObject buttonState = doc.createNestedObject("button_state");
  JsonVariant button = doc["button_state"];
  json_device_button(button);
  //JsonObject buttonState = doc.createNestedObject("button_state");
}

*/
void loop() {
  // Wait for an incomming connection
  EthernetClient client = server.available();
  api.checkEthernet(client);
  //if (client.connected()) {
    //app.process(&client);
    ///client.stop();
  //}
}
  
  
  
  

  // Create the "digital" array
  
  /*

  Serial.print(F("Sending: "));
  serializeJson(doc, Serial);
  Serial.println();

  // Write response headers
  client.println(F("HTTP/1.0 200 OK"));
  client.println(F("Content-Type: application/json"));
  client.println(F("Connection: close"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();

  // Write JSON document
  serializeJsonPretty(doc, client);

  // Disconnect
  client.stop();
}*/ 
