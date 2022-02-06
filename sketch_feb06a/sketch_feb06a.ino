// Ron Bentley, Stafford UK
// September 2021
//
// This example and code is in the public domain and may be used without
// restriction and without warranty.
//
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//         Example sketch - Multiple switches handled by a single interrupt
//         '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
// This sketch demonstrates how the ez_switch_lib may be used to handle multiple
// switches (button & toggle switches in this example)with a single interrupt routine.
//
// The use of the ez_switch_lib library for switches provides:
//   * switch type independence
//   * switch circuit type independence
//   * automatic multiple switch debounce handling
//   * parallel switching capabilities, and
//   * automatic interrupt handling for all switches
//
// The sketch will use digital pin 2 as the common interrupt pin and
// pins 3, 4, 5, 6, 7 and 8 as the switch pins.
//
// For an understanding of the full capabilities of the 'ez_switch_lib' library see
// the USER GUIDE:
// https://github.com/ronbentley1/ez_switch_lib-Arduino-Library/blob/main/ez_switch_lib_user_guide%2C%20v1.04.pdf
//
// or a synopsis CRIB SHEET:
// https://github.com/ronbentley1/ez_switch_lib-Arduino-Library/blob/main/ez_switch_lib%20crib%20sheet.pdf
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "ez_switch_lib.h"          // declare te switch library

#define common_interrupt_pin    2   // external (common) interrupt pin that all declared switches will use

#define num_switches            6   // number of switches the sketch will declare & use

Switches my_switches(num_switches); // create the 'Switches' instance for the given number of switches

// Declare the data required for each of our switches, 'my_switch_data':
// 'my_switch_data' layout, one row of data for each switch to be configured,
// as follows:
// [][0] = switch type (button_switch or toggle_switch)
// [][1] = digital pin connected to switch
// [][2] = the circuit type connecting the switch, here the first 3 switches
//         will have 10k ohm pull down resistors wired (circuit_C1 - INPUT type)
//         and the remainder none (circuit_C2 - INPUT_PULLUP type).
byte my_switch_data[][3] =
{
  button_switch,  3, circuit_C1, // this will be the switch entry for switch_id 0
  button_switch,  4, circuit_C1, // this will be the switch entry for switch_id 1
  button_switch,  5, circuit_C1, // etc
  button_switch,  6, circuit_C1,
  button_switch,  7, circuit_C1,
  button_switch,  8, circuit_C1,
};

void setup() {
  int result;
  Serial.begin(115200);
  // Add all switches to the library switch control structure we defined above
  // and link all switches to the same interrupt pin as a linked output
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
  // set debounce period
  my_switches.set_debounce(150);  // can be reduced if you have good quality switches
  // Now establish the common interrupt service routine (ISR) that
  // will be used for all declared switches.
  attachInterrupt(
    digitalPinToInterrupt(common_interrupt_pin),
    switch_ISR, // name of the sketch's ISR handler for switch interrupts
    RISING);    // trigger on a rising pin value
} // end of setup function

void loop() {
  // Keep testing switches, and let the interrupt handler do its thing
  // once a switch is switched to 'on'
  // We use 'read_switch' function as this deals with both switch types for us transparently.
  for (uint8_t switch_id = 0; switch_id < num_switches; switch_id++) {
    if (my_switches.read_switch(switch_id) == switched) {
      // *** Add any code here, if any, to process this switch as it has been actuated.
      // *** This is in addition to whatever the ISR does following the triggering
      // *** of the interrupt, if this switch has been linked to the common interrupt pin.

    }
  }
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Common ISR for handling interrupt triggers arising from associated switches
// when they transition to on then off for button switches and on or off
// for toggle switches.  The routine knows which switch has generated
// the interrupt because the ez_switch_lib switch read function records the
// actuated switch in the library variable 'last_switched_id'.
//
// The routine does nothing more than demonstrate the effectiveness of the
// use of a single ISR handling multiple switches by using the serial monitor
// to confirm correct operation.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
} // end of switch_ISR
