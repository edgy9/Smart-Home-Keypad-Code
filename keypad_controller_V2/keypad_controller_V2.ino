

bool sync_button = false;

void setup() {
   Serial.begin(9600);
   pinMode(2, OUTPUT);
   digitalWrite (2, HIGH );
   int devices[1];
   int current_id_count = 0;
   ping_devices();
   struct Device {
    int id;
    int type;
   };
}

void ping_devices(){
  digitalWrite(2, HIGH);
  int i = 1;
  while (i < 17) {
    Serial.print("I");
    Serial.print(i);
    Serial.print("P");
    
    digitalWrite(2, LOW);
    
    if(Serial.find("i")) {
      if(Serial.read() == i) {
        if(Serial.read() == "p"){
        int device_type=Serial.parseInt(); 
          if(Serial.read()=='f') //finish reading
          {
            //devices[i] = device_type;
          }
        } 
      }       
      }
    
    i = i + 1;
  }
}

void loop() {
  if (sync_button == true) {
    sync();
  }

}

void new_device() {
  int i = 1;
  while(i < 17){
    
  }
  int id = 10;
  Serial.print("I");
  Serial.print("N");
  Serial.print(id);
}

void sync() {
  
  Serial.print("I"); //initiate data packet
  Serial.print("S"); //code for Sync
  Serial.print("F"); //finish data packet
  Serial.flush();    
  
  digitalWrite(2, LOW);
  
  if(Serial.find("i")) {
      if(Serial.read() =='n') {
          if(Serial.read()=='f') //finish reading
       {
        
         digitalWrite(2, HIGH);
         new_device(); 
      }
      
  }

}
}
