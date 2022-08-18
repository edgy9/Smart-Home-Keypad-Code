#define rx_tx_pin               11
#define num_switches            4
const int device_id = 2;
const int device_type = 1;

int currentState;
int lastState = LOW;

bool synced = false;
bool canceled_sync = false;

void ping() {
    digitalWrite(rx_tx_pin,HIGH);
    Serial.print("i");
    Serial.print(device_id);
    Serial.print("p");
    Serial.print(device_type);
    Serial.print("f");
    Serial.flush();
    digitalWrite(rx_tx_pin,LOW);
}
void sync() {
    Serial.println("sinking");
    if (synced == false) {
        while (canceled_sync == false) {
            if (digitalRead(9) == HIGH) {                                         ////
               digitalWrite(rx_tx_pin, HIGH ); //enable to transmit
               Serial.print("i");
               Serial.print("z");
               Serial.print("s");
               Serial.print("n"); 
               Serial.print("f");
               Serial.flush(); 
               digitalWrite(rx_tx_pin, LOW ); 
    
               delay(50); 
    
               
               if(Serial.find("I")) {
                    if(Serial.read() == "N") {
                        if(Serial.read() == "P"){
                            int new_device_id=Serial.parseInt(); 
                            if(Serial.read()=='F') //finish reading
                                {
                                  const int device_id = new_device_id;
                                  synced = true;
                                }
                         } 
                     }       
                }
            } 
        }
    }
}


void setup() {
  Serial.begin(9600);
  pinMode(rx_tx_pin, OUTPUT);
  pinMode(6, INPUT);                                                                    
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  digitalWrite (rx_tx_pin, LOW );
  Serial.println("starting");
  
}

void loop() {
  digitalWrite(rx_tx_pin, LOW);
  if(synced == false){
    
    if ( Serial.available ()) {
                                                                                                                              TODO: FIX SYNCING 
        //Serial.print("serial avaibable");
        if ( Serial.read () == 'I' ) {
          char id = Serial.read();
          Serial.print(id);
              if (Serial.read() == 'Z'){       // z meaning id for all unadopted devices
                  char function = Serial.read ();
                  Serial.print(function);

                  if (function == 'S' ) {
                      if ( Serial.read () == 'F' ) {
                        Serial.print("sincing");
                        sync();
                        }
                      }
                  }
          }
    }
  }
          
  else if ( Serial.available ()) {
     if ( Serial.read () == 'I' ) {
          int id = Serial.parseInt();
          Serial.println(id);
              if (id == device_id){
                  char function = Serial.read ();
                  if (function == 'P'){
                    ping();
                    }
                  }
            }
        }
    
    if(synced){
      for (int switch_id = 0; switch_id < num_switches; switch_id++) {
        currentState = digitalRead(switch_id+6);
      
        if(lastState == LOW && currentState == HIGH){//pressed
          //Serial.print("pressed");
          if(synced == true) {
              digitalWrite(rx_tx_pin, HIGH);
              Serial.print("i");
              Serial.print(device_id);
              Serial.print("p");
              Serial.print(switch_id); // the id of the last triggering switch
              Serial.print("f");
              Serial.flush();
              digitalWrite(rx_tx_pin, LOW);
              delay(50);
            }
       
         while(digitalRead(switch_id+6) != LOW){}//released
         if(synced == true) {
              digitalWrite(rx_tx_pin, HIGH);
              Serial.print("i");
              Serial.print(device_id);
              Serial.print("r");
              Serial.print(switch_id); // the id of the last triggering switch
              Serial.print("f");
              Serial.flush();
              digitalWrite(rx_tx_pin, LOW);
            }
          }
          lastState = currentState;
      }
    }
}
