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
  pinMode(SR.OE, OUTPUT);
  pinMode(SR.RCLK, OUTPUT);
  pinMode(SR.SRCLK, OUTPUT);
  pinMode(SR.SRCLR, OUTPUT);
  SR.disable();
  SR.clear();
  Serial.begin(9600); 
  delay(1000);

}

void selfTestIndividual(){
  Serial.println("Begining self-test-individual");
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 12; j++){
      writeDot(j, i);
      delay(1000);
    }
  }
}

void flashDisplay(){
  SR.enable();
  delay(2);
  SR.disable();
}



void writeOutput(char location, bool state){ 
  SR.disable(); 
  SR.clear();
  for(short i = 0; i < 12; i++){
    if(i == location && state){
      digitalWrite(SR.serial, HIGH);
      SR.incrementSR();
      SR.incrementR();
    } else if (i == location && !state){
      digitalWrite(SR.serial, LOW);
      SR.incrementSR();
      SR.incrementR();
    }
  }
  flashDisplay();
  return;
}

void loop() {
  
  delay(30000);
}