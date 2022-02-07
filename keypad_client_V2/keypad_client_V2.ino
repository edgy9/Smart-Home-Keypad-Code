#include "ez_switch_lib.h"

#define rx_tx_pin               11
#define common_interrupt_pin    2
#define num_switches            8
//const int device_id = 0;
const int device_type = 1;

bool synced = false;
bool canceled_sync = false;

Switches my_switches(num_switches); // create the 'Switches' instance for the given number of switches

byte my_switch_data[][3] =
{
  button_switch,  3, circuit_C1, // this will be the switch entry for switch_id 0
  button_switch,  4, circuit_C1, // this will be the switch entry for switch_id 1
  button_switch,  5, circuit_C1, // etc
  button_switch,  6, circuit_C1,
  button_switch,  7, circuit_C1,
  button_switch,  8, circuit_C1,
  button_switch,  9, circuit_C1,
  button_switch, 10, circuit_C1,
};




void ping() {
    digitalWrite(rx_tx_pin,HIGH);
    Serial.write("i");
    Serial.write(device_id);
    Serial.write("p");
    Serial.write(device_type);
    Serial.flush();
    digitalWrite(rx_tx_pin,LOW);
}
void sync() {
    if (synced == false) {
        synced = true;
        while (canceled_sync == false) {
            if (digitalRead(3) == HIGH) {                                         ////
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
                                }
                         } 
                     }       
                }
            } 
        }
    }
}
void setup() {
   int result;
  Serial.begin(9600);
  pinMode(rx_tx_pin, OUTPUT);
  digitalWrite (rx_tx_pin, LOW );
  Serial.println("starting");
  for (uint8_t switch_id = 0; switch_id < num_switches; switch_id++) {
    result = my_switches.add_switch(
               my_switch_data[switch_id][0],  // switch type
               my_switch_data[switch_id][1],  // digital pin switch is wired to
               my_switch_data[switch_id][2]); // type of circuit switch is wired as
    // Check that we have successfully added the current switch or not
    if (result < 0) {
      // No slots left or bad parameters
      Serial.println("*** Failure to add switch.\n Terminated");
      Serial.flush();
      exit(0);
    }
    // now make the linkage for this switch_id to the common interrupt pin
    result = my_switches.link_switch_to_output(
               switch_id,             // switch to be linked
               common_interrupt_pin,  // digital pin to link to for interrupt
               LOW);                  // start with interrupt pin LOW, as interrupt will be triggered on RISING
    // Check that we have successfully linked the current switch or not
    if (result < 0) {
      // Could not link the given switch
      Serial.println("*** Failure to link switch.\n Terminated");
      Serial.flush();
      exit(0);
    }
  }

  my_switches.set_debounce(150);
  
  attachInterrupt(
    digitalPinToInterrupt(common_interrupt_pin),
    switch_ISR, // name of the sketch's ISR handler for switch interrupts
    RISING);    // trigger on a rising pin value
}


void loop() {
  if(synced == False){
      if ( Serial.available ()) {
        if ( Serial.read () == 'I' ) {
          char id = Serial.read();
          if (id == "Z"){       // z meaning id for all unadopted devices
              char function = Serial.read ();
              if (function == 'S' ) {
                  if ( Serial.read () == 'F' ) {
                    sync();
                    }
                  }
              }
          
          
          if (id == device_id){
              if (function == 'P'){
                ping();
                }
              }
            }
      }
    }
  else{
    
  }
}

for (uint8_t switch_id = 0; switch_id < num_switches; switch_id++) {
    if (my_switches.read_switch(switch_id) == switched) {
      
      // *** Add any code here, if any, to process this switch as it has been actuated.
      // *** This is in addition to whatever the ISR does following the triggering
      // *** of the interrupt, if this switch has been linked to the common interrupt pin.

    }
}
}


void switch_ISR()
{
  // Reset the interrupt pin to LOW, so that any other switch will fire the
  // interrupt whist one or more switches complete their transition stage
  byte switch_id = my_switches.last_switched_id;  // switch id of switch currently switched
  digitalWrite(my_switches.switches[switch_id].switch_out_pin, LOW);
  // Reset the soft status of the switch setting to ensure that we get an interrupt event
  // on the linked interrupt pin at next switch change
  my_switches.switches[switch_id].switch_out_pin_status = LOW;
  Serial.print("** Interrupt triggered for switch id: ");
  Serial.print(switch_id); // the id of the last triggering switch
  Serial.println();
  Serial.flush();
}
