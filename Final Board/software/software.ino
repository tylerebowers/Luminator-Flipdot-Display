struct {
  short DI = 23;
  short CLK = 21;
  short CS = 19;
  short EN = 22;
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
  pinMode(SR.DI, OUTPUT);
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
      digitalWrite(SR.DI, HIGH);
      SR.incrementCLK();
    } else {
      digitalWrite(SR.DI, LOW);
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
  digitalWrite(SR.DI, LOW);
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
  //Serial.printf("WROTE (%d:%d): \n",location,state);
  digitalWrite(SR.CS, LOW);
  for(short i = 15; i >=0 ; i--){
    if(toWrite & (1UL<<i)){
      //Serial.print("1");
      digitalWrite(SR.DI, HIGH);
      SR.incrementCLK();
    } else {
      //Serial.print("0");
      digitalWrite(SR.DI, LOW);
      SR.incrementCLK();
    }
  }
  digitalWrite(SR.CS, HIGH);
  //Serial.println();
  return;
}

void loop() {
  //Serial.println("start");
  /*
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 8; j++){
      clear();
      delay(1000);
      writeOutput(j, i);
      delay(1000);
    }
  }
  //Serial.println("end");
  delay(1000);
  */
  writeOutput(0,1);
  delayMicroseconds(120);
  writeOutput(0,0);
  delayMicroseconds(120);
}