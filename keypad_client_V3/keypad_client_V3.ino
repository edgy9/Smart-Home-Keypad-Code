#define rx_tx_pin               11
#define num_switches            7
const int device_id = 2;
const int device_type = 1;

unsigned long debounce_delay = 50;
unsigned long last_debounce_time[7] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0
  };


int currentState;
int true_current_state;

bool synced = true;
bool canceled_sync = false;
bool counting[7] = {
  false,
  false,
  false,
  false,
  false,
  false,
  false
};

int lastState[7] = {
LOW,
LOW,
LOW,
LOW,
LOW,
LOW,
LOW};
void ping() {
    digitalWrite(rx_tx_pin,HIGH);
    Serial.print('i');
    Serial.print(device_id);
    Serial.print('p');
    Serial.print(device_type);
    Serial.print('f');
    Serial.flush();
    digitalWrite(rx_tx_pin,LOW);
}
void sync() {
    Serial.println("sinking");
    if (synced == false) {
        while (canceled_sync == false) {
            if (digitalRead(9) == HIGH) {                                         ////
               digitalWrite(rx_tx_pin, HIGH ); //enable to transmit
               Serial.print('i');
               Serial.print('z');
               Serial.print('s');
               Serial.print('n'); 
               Serial.print('f');
               Serial.flush(); 
               digitalWrite(rx_tx_pin, LOW ); 
    
               delay(50); 
    
               return;
               if(Serial.find('I')) {
                    if(Serial.read() == 'N') {
                        if(Serial.read() == 'P'){
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
                                                                      
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);                                                                    
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  digitalWrite (rx_tx_pin, LOW );
  Serial.println("starting");
  
}

void loop() {
  digitalWrite(rx_tx_pin, LOW);
  if(synced == false){
    
    if ( Serial.available ()) {
        //Serial.print(Serial.peek());                                                                                                                  //    TODO: FIX SYNCING 
        //Serial.print("serial avaibable");
        
        if ( Serial.find('Z')) {
          //Serial.print(Serial.read ());
            char id = Serial.read();
              if (id == ('I')){       // z meaning id for all unadopted devices
                Serial.print("found");
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
        currentState = digitalRead(switch_id+3);
        
      
        if(lastState[switch_id] == LOW && currentState == HIGH && counting[switch_id] == false){//pressed            
          counting[switch_id] = true;//counting debounce
          last_debounce_time[switch_id] = millis();
          digitalWrite(rx_tx_pin, HIGH);
          Serial.print('i');
          Serial.print(device_id);
          Serial.print('p');
          Serial.print(switch_id); // the id of the last triggering switch
          Serial.print('f');
          Serial.flush();
          digitalWrite(rx_tx_pin, LOW);
        }
        if(counting[switch_id]){
        if(currentState == LOW && ((millis() - last_debounce_time[switch_id]) > debounce_delay)){// released and out of debounce time
                
            digitalWrite(rx_tx_pin, HIGH);
            Serial.print('i');
            Serial.print(device_id);
            Serial.print('r');
            Serial.print(switch_id); // the id of the last triggering switch
            Serial.print('f');
            Serial.flush();
            digitalWrite(rx_tx_pin, LOW);
            counting[switch_id] = false;
            }
          }
          lastState[switch_id] = currentState;
      }
    }
}
