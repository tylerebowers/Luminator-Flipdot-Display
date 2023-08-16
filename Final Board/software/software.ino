struct {
  short serial = D8;
  short CLK = D7;
  short CS = D6;
  short EN = D5;
  void disable() {
    digitalWrite(EN, LOW); // disable
  }
  void enable(){
    digitalWrite(EN, HIGH); // enable
  }
  void incrementCLK(){
    digitalWrite(CLK, HIGH);
    digitalWrite(CLK, LOW);
  }
} SR;

void setup() {
  pinMode(SR.serial, OUTPUT);
  pinMode(SR.CLK, OUTPUT);
  pinMode(SR.CS, OUTPUT);
  pinMode(SR.EN, OUTPUT);
  SR.enable();
  Serial.begin(9600); 
}

void writeSerial(short toWrite){
  digitalWrite(SR.CS, LOW);
  for(short i = 15; i >=0 ; i--){
    if(toWrite & (1UL<<i)){
      digitalWrite(SR.serial, HIGH);
      SR.incrementCLK();
    } else {
      digitalWrite(SR.serial, LOW);
      SR.incrementCLK();
    }
  }
  digitalWrite(SR.CS, HIGH);
}

void clear(){
  writeSerial(0);
  writeSerial(16384);
}


void writeOutput(char location, bool state){ 
  digitalWrite(SR.CLK, LOW);
  digitalWrite(SR.serial, LOW);
  short toWrite = 0;
  if(location <=5) {
    toWrite |= 1UL << location+7;
    if(state){
      toWrite |= 1UL << location+1;
    }
  } else if(location >= 6) {
    toWrite |= 1UL << 14;
    toWrite |= 1UL << location+1;
    if(state){
      toWrite |= 1UL << location-5;
    }
  }
  Serial.printf("WROTE (%d:%d): \n",location,state);
  digitalWrite(SR.CS, LOW);
  for(short i = 15; i >=0 ; i--){
    if(toWrite & (1UL<<i)){
      Serial.print("1");
      digitalWrite(SR.serial, HIGH);
      SR.incrementCLK();
    } else {
      Serial.print("0");
      digitalWrite(SR.serial, LOW);
      SR.incrementCLK();
    }
  }
  digitalWrite(SR.CS, HIGH);
  Serial.println();
  return;
}

void loop() {
  //Serial.println("start");
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 8; j++){
      clear();
      writeOutput(j, i);
      delay(4000);
    }
  }
  //Serial.println("end");
  delay(1000);
}