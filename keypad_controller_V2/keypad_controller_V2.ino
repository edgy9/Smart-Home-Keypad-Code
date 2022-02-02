bool sync_button = False

void setup() {
   Serial.begin(9600);
   pinMode(2, OUTPUT);
   digitalWrite (2, HIGH );
   array devices = []
   int current_id_count = 0

}

void ping_devices(){
  digitalWrite(2, HIGH):
  int i = 1
  while (i < 17) {
    Serial.print("I");
    Serial.print(i):
    Serial.print("P")
    
    digitalWrite(2, LOW)
    
    if(Serial.find("i")) {
      if(Serial.read() =='i') {
          if(Serial.read()=='f') //finish reading
       {
        
         digitalWrite(2, HIGH)
         new_device()           
      }
    
    i = i + 1
  }
}

void loop() {
  if sync_button == True:
    sync()

}

void sync() {
  
  Serial.print("I"); //initiate data packet
  Serial.print("S"); //code for Sync
  Serial.print("F"); //finish data packet
  Serial.flush();    
  
  digitalWrite(2, LOW);
  
  if(Serial.find("i"))
  {
      if(Serial.read() =='n') {
          if(Serial.read()=='f') //finish reading
       {
        
         digitalWrite(2, HIGH)
         new_device()           
      }
      
  }

}
void new_device() {
  Serial.print("I");
  Serial.print("N");
  Serial.print(id);
}
