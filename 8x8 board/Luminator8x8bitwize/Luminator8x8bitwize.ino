//this program opperates based on matrix values
//it is memory inefficient the bitwize version uses ~8x less ram.

#define NUMROWS 8
#define NUMCOLS 8
#define FLASHTIME 1   //in milliseconds
#define DEBUG 1


struct shiftRegister{
  short serial;
  short OE;
  short RCLK;
  short SRCLK;
  short SRCLR;
  short rank;
  void disable() {
    digitalWrite(OE, HIGH); // disable
  }
  void enable(){
    digitalWrite(OE, LOW); // enable
  }
  void clear(){
    digitalWrite(SRCLR, LOW); // clear
    digitalWrite(SRCLR, HIGH); // clear
  }
  void incrementSR(){
    digitalWrite(SRCLK, HIGH);
    digitalWrite(SRCLK, LOW);
  }
  void incrementR(){
    digitalWrite(RCLK, HIGH);
    digitalWrite(RCLK, LOW);
  }
  void setIdle(){
    digitalWrite(serial, LOW);
  }
} col, row;
/*
void bitsToBytes(unsigned short from[NUMCOLS][NUMROWS], unsigned short to[NUMCOLS]){
  int sum;
  for(short i = 0; i < 8; i++){
    sum = 0;
    for(short j = 0; j < 8; j++){
        sum = sum << 1UL;
        sum |= from[i][j] & 1;
      }
    to[i] = sum;
  }
}
*/
/*
void writeColumnBits(unsigned short bitwize, short state){
  col.disable();
  col.clear();
  for(short i = 1; i <= NUMROWS/8; i++){
    for(short j = 8; j > 0; j--){ // writes +POS (83) (ON)
      if(state == 1 && (bitwize >> ((8*i)-j) & 1)){    //array[(8*i)-j] == 1 
        digitalWrite(col.serial, HIGH);
      }
      else{
        digitalWrite(col.serial, LOW);
      }
      col.incrementSR();
      col.incrementR();
    }
    for(short k = 8; k > 0; k--){ // writes -GND (81) (OFF)
      if(state == 0 && (bitwize >> ((8*i)-k) & 0)){     //array[(8*i)-k] == 0
        digitalWrite(col.serial, HIGH);
      }
      else{
        digitalWrite(col.serial, LOW);
      }
      col.incrementSR();
      col.incrementR();
    }
  }
  col.setIdle();
}
*/
void writeColumnSingle(short index, short state){
  col.disable();
  col.clear();
  for(short i = 1; i <= NUMCOLS/8; i++){
    for(short j = 8; j > 0; j--){ // writes +POS (83) (ON)
      if((state == 1) && ((8*i)-j == index)){
        digitalWrite(col.serial, HIGH);
      }
      else{
        digitalWrite(col.serial, LOW);
      }
      col.incrementSR();
      col.incrementR();
    }
    for(short k = 8; k > 0; k--){ // writes -GND (81) (OFF)
      if((state == 0) && ((8*i)-k == index)){
        digitalWrite(col.serial, HIGH);
      }
      else{
        digitalWrite(col.serial, LOW);
      }
      col.incrementSR();
      col.incrementR();
    }
  }
  col.setIdle();
}
/*
void writeRowBits(unsigned short bitwize, short state){
  row.disable();
  row.clear();
  for(short i = 1; i <= NUMCOLS/8; i++){
    for(short j = 8; j > 0; j--){ // writes +POS (83) (OFF)
      if(state == 0 && (bitwize >> ((8*i)-j) & 0)){
        digitalWrite(row.serial, HIGH);
      }
      else{
        digitalWrite(row.serial, LOW);
      }
      row.incrementSR();
      row.incrementR();
    }
    for(short k = 8; k > 0; k--){ // writes -GND (81) (ON)
      if(state == 1 && (bitwize >> ((8*i)-k) & 1)){
        digitalWrite(row.serial, HIGH);
      }
      else{
        digitalWrite(row.serial, LOW);
      }
      row.incrementSR();
      row.incrementR();
    }
  }
  row.setIdle();
}
*/
void writeRowSingle(short index, short state){
  row.disable();
  row.clear();
  for(short i = 1; i <= NUMCOLS/8; i++){
    for(short j = 8; j > 0; j--){ // writes +POS (83) (OFF)
      if(state == 0 && (8*i)-j == index){
        digitalWrite(row.serial, HIGH);
      }
      else{
        digitalWrite(row.serial, LOW);
      }
      row.incrementSR();
      row.incrementR();
    }
    for(short k = 8; k > 0; k--){ // writes -GND (81) (ON)
      if(state == 1 && (8*i)-k == index){
        digitalWrite(row.serial, HIGH);
      }
      else{
        digitalWrite(row.serial, LOW);
      }
      row.incrementSR();
      row.incrementR();
    }
  }
  row.setIdle();
}

void flashDisplay(){
  row.enable();
  col.enable();
  delay(FLASHTIME);
  col.disable();
  row.disable();
}
/*
void writeDisplay(short array[NUMCOLS]){ // dont use (this version is bad for the drivers)
  for(short i = 0; i < NUMCOLS; i++){
    writeRowBits(array[i], 1);
    writeColumnSingle(i, 1);
    flashDisplay();  
    delay(FLASHTIME);  
    writeRowBits(array[i], 0);
    writeColumnSingle(i, 0);
    flashDisplayLong();
  }
}
*/
void writeDisplay(short array[NUMCOLS]){ // one at a time
  for(short i = 0; i < NUMCOLS; i++){ // For each column
    for(short j = 0; j < NUMROWS; j++){ // do each row 
      if(((array[i] >> j) & 1)){
        writeColumnSingle(i, 1);
        writeRowSingle(j, 1);
        flashDisplay(); 
      } else {
        writeColumnSingle(i, 0);
        writeRowSingle(j, 0);
        flashDisplay();
      }
    }
  }
}
void setup() {
  Serial.begin(9600);
  col.serial = 2;
  col.RCLK = 3;
  col.SRCLK = 4;
  col.OE = 5;
  col.SRCLR = 6;
  col.rank = NUMCOLS;
  pinMode(col.serial, OUTPUT);
  pinMode(col.OE, OUTPUT);
  pinMode(col.RCLK, OUTPUT);
  pinMode(col.SRCLK, OUTPUT);
  pinMode(col.SRCLR, OUTPUT);
  col.disable();
  col.clear();
  row.serial = 7;
  row.RCLK = 8;
  row.SRCLK = 9;
  row.OE = 10;
  row.SRCLR = 11;
  row.rank = NUMROWS;
  pinMode(row.serial, OUTPUT);
  pinMode(row.OE, OUTPUT);
  pinMode(row.RCLK, OUTPUT);
  pinMode(row.SRCLK, OUTPUT);
  pinMode(row.SRCLR, OUTPUT);
  row.disable();
  row.clear();
}

void loop() {
  /*
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      //delay(500);
      writeRowSingle(i, 1);
      writeColumnSingle(j, 1);
      flashDisplay(); 
    }
  }
  delay(500);
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      //delay(500);
      writeRowSingle(i, 0);
      writeColumnSingle(j, 0);
      flashDisplay(); 
    }
  }
  delay(500);
  */
  /*
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      delay(500);
      writeRowSingle(i, 1);
      writeColumnSingle(j, 1);
      flashDisplay(); 
      delay(500);
      writeRowSingle(i, 0);
      writeColumnSingle(j, 0);
      flashDisplay();
    }
  }
  */
  
  unsigned short circle[NUMCOLS] = {0,24,36,66,66,36,24,0};  // each number representa a column for 8 rows it is 1 byte (char), 16 rows 2 bytes (short), etc
  unsigned short blank[NUMCOLS] = {0,0,0,0,0,0,0,0};
  unsigned short full[NUMCOLS] = {255,255,255,255,255,255,255,255};
  unsigned short north[NUMCOLS] = {}
  unsigned short south[NUMCOLS] = {}
  unsigned short east[NUMCOLS] = {}
  unsigned short west[NUMCOLS] = {}
  //delay(1000);
  writeDisplay(blank);
  delay(3000);
  writeDisplay(circle);
  delay(3000);
  writeDisplay(full);
  delay(3000);
}
