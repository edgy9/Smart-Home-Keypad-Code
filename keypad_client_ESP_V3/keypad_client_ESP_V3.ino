#define rx_tx_pin               4
#define num_switches            12
const int device_id = 3;
const int device_type = 1;

unsigned long debounce_delay = 50;
unsigned long last_debounce_time[num_switches] = {
  0,
  0,
  0,
  0,
  0,
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
bool counting[num_switches] = {
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false
};

int lastState[12] = {
LOW,
LOW,
LOW,
LOW,
LOW,
LOW,
LOW,
LOW,
LOW,
LOW,
LOW,
LOW};

int switch_pins[12] = {
  34, 
  35, 
  25, 
  26,                                                                     
  27, 
  14, 
  22, 
  21, 
  19, 
  18, 
  17, 
  16
};
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
            if (digitalRead(16) == HIGH) {                                         ////
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
                                  Serial.println(device_id);
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
  Serial.begin(115200);
  delay(5000);
  pinMode(rx_tx_pin, OUTPUT);
  digitalWrite (rx_tx_pin, LOW ); 
  delay(100);
  digitalWrite (rx_tx_pin, HIGH );  
  delay(100);
  digitalWrite (rx_tx_pin, LOW );
  Serial.println("starting keypad client esp v3");
                                                    
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(25, INPUT);
  pinMode(26, INPUT);                                                                    
  pinMode(27, INPUT);
  pinMode(14, INPUT);
  pinMode(22, INPUT);
  pinMode(21, INPUT);
  pinMode(19, INPUT);
  pinMode(18, INPUT);
  pinMode(17, INPUT);
  pinMode(16, INPUT);
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
      for (int i = 0; i < 12; i++) {
        int switch_id = switch_pins[i];
        currentState = digitalRead(switch_id);
        
      
        if(lastState[i] == LOW && currentState == HIGH && counting[i] == false){//pressed            
          counting[i] = true;//counting debounce
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
        if(counting[i]){
        if(currentState == LOW && ((millis() - last_debounce_time[i]) > debounce_delay)){// released and out of debounce time
                
            digitalWrite(rx_tx_pin, HIGH);
            Serial.print('i');
            Serial.print(device_id);
            Serial.print('r');
            Serial.print(switch_id); // the id of the last triggering switch
            Serial.print('f');
            Serial.flush();
            digitalWrite(rx_tx_pin, LOW);
            counting[i] = false;
            }
          }
          lastState[i] = currentState;
      }
    }
}
