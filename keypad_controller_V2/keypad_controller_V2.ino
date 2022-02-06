
bool sync_button = false;


int online =     1;
int offline =    0;
byte device_list[][3] =   //id 
                          //device type 0 = unasigned, 1 = keypad
{
  0,  0,  offline,
  1,  0,  offline,
  2,  0,  offline,
  3,  0,  offline,
  4,  0,  offline,
  5,  0,  offline,
  6,  0,  offline,
  7,  0,  offline,
  8,  0,  offline,
  9,  0,  offline,
  10, 0,  offline,
  11, 0,  offline,
  12, 0,  offline,
  13, 0,  offline,
  14, 0,  offline,
  15, 0,  offline,
};



void ping_devices(){
  digitalWrite(2, HIGH);
  int i = 1;
  while (i < 17) {
    i = i + 1;
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
              device_list[i][1] = device_type;
              device_list[i][2] = online;
            }
          } 
         }       
     }
     else{
          device_list[i][2] = offline;
     }
    
    
  }
}
void setup() {
   Serial.begin(9600);
   pinMode(2, OUTPUT);
   digitalWrite (2, HIGH );
   
   ping_devices();
   
}
void loop() {
  if (sync_button == true) {
    sync();
  }

}

void new_device() {
  int i = 1;
  ping_devices();
  while(i < 17){
    
  }
  int id = 10;
  Serial.print("I");
  Serial.print("N");
  Serial.print(id);
}

void sync() {
  
  Serial.print("I"); //initiate data packet
  Serial.print("Z"); //code for unassigned
  Serial.print("S"); //code for Sync
  Serial.print("F"); //finish data packet
  Serial.flush();    
  
  digitalWrite(2, LOW);
  
  if(Serial.find("i")) {
      if(Serial.read() =='z') {
          if(Serial.read() =='n') {
              if(Serial.read() =='n') {
                  if(Serial.read()=='f') //finish reading
                      {
                
                         digitalWrite(2, HIGH);
                         new_device(); 
                      }
              } 
          }
      
      }

  }
}
