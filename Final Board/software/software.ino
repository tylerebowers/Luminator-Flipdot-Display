struct {
  short serial = D8;
  short CLK = D7;
  short CS = D6;
  short OE = D5;
  void disable() {
    digitalWrite(OE, LOW); // disable
  }
  void enable(){
    digitalWrite(OE, HIGH); // enable
  }
  void clear(){
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
  pinMode(SR.OE, OUTPUT);
  SR.disable();
  //SR.clear();
  Serial.begin(9600); 
}

void flashDisplay(){
  SR.enable();
  delay(2);
  SR.disable();
}



void writeOutput(char location, bool state){ 
  //SR.disable(); 
  //SR.clear();
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
  for(short i = 0; i < 16; i++){
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
  Serial.println();
  return;
}

void loop() {
  digitalWrite(SR.CS, LOW);
  SR.enable();
  Serial.println("start");
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 8; j++){
      writeOutput(j, i);
      //SR.enable();
      delay(1000);
    }
  }
  Serial.println("end");
  delay(1000);
}