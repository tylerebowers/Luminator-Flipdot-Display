struct {
  short DI = 19;
  short CLK = 23;
  short EN = 5;
  short DO = 18;
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
} PSR;

struct {
  //short SEL = ;
  //short OE = ;
  //short RCLK = ;
  //short SRCLK = ;
  //short SRCLR = ;
  
  short COL = 17;
  short ROW = 16;
  void select(char rc){
    if (rc == 'r'){digitalWrite(ROW, LOW);digitalWrite(COL, HIGH);}
    else if (rc == 'c'){digitalWrite(ROW, HIGH);digitalWrite(COL, LOW);}
    else {digitalWrite(ROW, HIGH);digitalWrite(COL, HIGH);}
  }

} SEL;

void setup() {
  pinMode(PSR.DI, OUTPUT);
  pinMode(PSR.CLK, OUTPUT);
  pinMode(PSR.EN, OUTPUT);
  PSR.enable();

  pinMode(SEL.COL, OUTPUT);
  pinMode(SEL.ROW, OUTPUT);
  Serial.begin(9600); 
}

/*
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
*/

void changeOutput(char location, char state){ // state: 'h' high, 'l' low, 'o' open
  digitalWrite(PSR.CLK, LOW);
  digitalWrite(PSR.DI, LOW);
  short toWrite = 0;
  if(location <=5) {
    if((state == 'h') | (state == 'l')){
      toWrite |= 1UL << location+7;
    }
    if((state == 'h')){
      toWrite |= 1UL << location+1;
    }
  } else if(location >= 6) {
    toWrite |= 1UL << 14;
    if((state == 'h') | (state == 'l')){
      toWrite |= 1UL << location+1;
    }
    if(state == 'h'){
      toWrite |= 1UL << location-5;
    }
  }
  Serial.printf("WROTE (%d:%c): \n",location,state);
  for(short i = 15; i >=0 ; i--){
    if(toWrite & (1UL<<i)){
      Serial.print("1");
      digitalWrite(PSR.DI, HIGH);
      PSR.incrementCLK();
    } else {
      Serial.print("0");
      digitalWrite(PSR.DI, LOW);
      PSR.incrementCLK();
    }
  }
  Serial.println();
  return;
}

void changeDot(char row, char col, bool state){
  if (state){
    //row on
    SEL.select('r');
    changeOutput(row, 'l');
    //col on
    SEL.select('c');
    changeOutput(col, 'h');
    //delay
    delayMicroseconds(1000);
    //col off
    SEL.select('c');//unnessesary
    changeOutput(col, 'o');
    //row off
    SEL.select('r');
    changeOutput(row, 'o');
    SEL.select('n');
  } else {
    //row on
    SEL.select('r');
    changeOutput(row, 'h');
    //col on
    SEL.select('c');
    changeOutput(row, 'l');
    //delay
    delayMicroseconds(1000);
    //col off
    SEL.select('c');//unnessesary
    changeOutput(row, 'o');
    //row off
    SEL.select('r');
    changeOutput(row, 'o');
    SEL.select('n');
  }
}

void loop() {
  //for (int i = 0, i<8, i++){}
  changeDot(0,0,1);
  delay(2500);
  changeDot(0,0,0);
  delay(2500);
}