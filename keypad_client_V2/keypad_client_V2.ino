void setup() {
  Serial.begin(9600);
  bool synced = false;
  bool canceled_sync = false;
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(2, OUTPUT);
  digitalWrite (2, LOW );
  int device_id = 1;
  int device_type = 1;
  

}

void loop() {

  if ( Serial.available ()) {
    if ( Serial.read () == 'I' ) {
      char id = Serial.read();
      if (id == Z){
        char function = Serial.read ();
        if (function == 'S' ) {
          if ( Serial.read () == 'F' ) {
            sync()
          }
        }
      }
      
      if (id == device_id){
          if (function == 'P'){
            ping()
          }
        }
    }
    void ping() {
      digitalWrite(2,HIGH);
      Serial.write("i");
      Serial.write(device_id);
      Serial.write("p");
      Serial.write(device_type);
      Serial.flush();
      digitalWrite(2,LOW);
    }
    void sync() {
      if (synced == false) {
        synced = True
        while (canceled_sync == false) {
          if (digitalRead(3) == HIGH) {
             digitalWrite(2, HIGH ); //enable to transmit
             Serial.print( "i" ); 
             Serial.println( "s" );
             Serial.println("n"); 
             Serial.flush(); 
             digitalWrite(2, LOW ); 
          }
          
        }

      }


    }
