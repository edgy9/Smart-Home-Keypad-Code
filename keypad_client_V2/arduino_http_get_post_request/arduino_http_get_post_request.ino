#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetServer server(80);

void setup() {
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial) continue;

  // Initialize Ethernet libary
  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize Ethernet library"));
    return;
  }

  // Start to listen
  server.begin();

  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Wait for an incomming connection
  EthernetClient client = server.available();

  // Do we have a client?
  if (!client) return;

  Serial.println(F("New client"));

  // Read the request (we ignore the content in this example)
  while (client.available()) client.read();

  // Allocate a temporary JsonDocument
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<500> doc;

  // Create the "analog" array
  JsonObject buttonState = doc.createNestedObject("button_state");
  buttonState["1"] = analogRead(1);
  
  buttonState["2"] = analogRead(2);
  
  buttonState["3"] = analogRead(3);
  
  buttonState["4"] = analogRead(4);
  
  buttonState["5"] = analogRead(5);
  
  buttonState["6"] = analogRead(1);
  
  buttonState["7"] = analogRead(2);
  
  
  
  

  // Create the "digital" array
  JsonObject deviceInfo = doc.createNestedObject("device_info");
  JsonArray device_ip = deviceInfo.createNestedArray("ip");
  
    device_ip.add(Ethernet.localIP());
  

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
}
