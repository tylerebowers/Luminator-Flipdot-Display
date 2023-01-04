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

void writeDisplayFull(char array[NUMCOLS]){ // one col at a time
  for(short i = 0; i < NUMCOLS; i++){ // For each column
    for(short j = 0; j < NUMROWS; j++){ // do each row 
      if(((array[i] >> j) & 1)){
        writeColumnSingle(i, 1);
        writeRowSingle(NUMROWS-j-1, 1);
        flashDisplay(); 
      } else {
        writeColumnSingle(i, 0);
        writeRowSingle(NUMROWS-j-1, 0);
        flashDisplay();
      }
    }
  }
}

void writeDisplay(byte * array, int x, int y){ // one dot at a time
  for(short i = 0; i < x; i++){ // For each column
    for(short j = y-1; j >= 0; j--){ // do each row 
      if(((array[i] >> j) & 1)){
        writeColumnSingle(i, 1);
        writeRowSingle(y-j-1, 1);
        flashDisplay(); 
      } else {
        writeColumnSingle(i, 0);
        writeRowSingle(y-j-1, 0);
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


unsigned char blank[NUMCOLS] = {0,0,0,0,0,0,0,0};
unsigned char full[NUMCOLS] = {255,255,255,255,255,255,255,255};
unsigned char confetti[NUMCOLS] = {170, 85, 170, 85, 170, 85, 170, 85};
unsigned char confetti2[NUMCOLS] = {85, 170, 85, 170, 85, 170, 85, 170};


void loop() {
  delay(1000);
  writeDisplayFull(blank);
  delay(500);
  for(int i = 0; i < 10; i++){
    writeDisplayFull(confetti);
    delay(200);
    writeDisplayFull(confetti2);
    delay(200);
  }
  writeDisplayFull(blank);
  delay(500);
  unsigned char animation[12][8] = {{0, 0, 0, 192, 192, 0, 0, 0}, {0, 0, 0, 96, 96, 0, 0, 0}, {0, 0, 0, 48, 48, 0, 0, 0}, {0, 0, 0, 24, 24, 0, 0, 0}, {0, 0, 0, 12, 12, 0, 0, 0}, {0, 0, 0, 6, 6, 0, 0, 0}, {0, 0, 0, 3, 3, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 5, 0, 0, 5, 0, 0}, {0, 9, 0, 0, 0, 0, 9, 0}, {5, 0, 0, 0, 0, 0, 0, 5}, {0, 0, 0, 0, 0, 0, 0, 0}};
  unsigned char aniSteps[27] = {1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 4, 5, 6, 7, 8, 7, 6, 5, 6, 7, 9, 10, 11, 12};
  for(int i = 0; i < 10; i++){
    for(int i = 0; i < 27; i++){
      writeDisplayFull(animation[aniSteps[i]-1]);
      delay(5);
    }
  }

}
