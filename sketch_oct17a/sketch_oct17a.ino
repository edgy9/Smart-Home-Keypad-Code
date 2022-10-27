const int EnTxPin =  5;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(EnTxPin, OUTPUT);
digitalWrite(EnTxPin, HIGH); 
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print("hello");
delay(300);
}
