#define FLASHTIME 200   //in microseconds

struct Columns{
  uint8_t SER = 23;
  uint8_t OE = 19;
  uint8_t RCLK = 18;
  uint8_t SRCLK = 17;
  uint8_t SRCLR = 16;
  uint8_t num = 32;
  uint8_t numOnBoard = 28;
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
    digitalWrite(SER, LOW);
  }
} cols;

struct Rows{
  uint8_t SER = 32;
  uint8_t OE = 33;
  uint8_t RCLK = 25;
  uint8_t SRCLK = 26;
  uint8_t SRCLR = 27;
  uint8_t num = 16;
  uint8_t numBoards = 4;
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
    digitalWrite(SER, LOW);
  }
} rows;


void toggleColumn(short c, bool state){
  cols.disable();
  cols.clear();
  c = c%cols.numOnBoard;
  for(int8_t i = 3; i >=0; i--){
    short c_offset = i*8;
    for(int8_t j = 7; j >= 0; j--){ // writes +POS (83) (ON)
      if((j+c_offset == c) && state){
        digitalWrite(cols.SER, HIGH);
      }
      else{
        digitalWrite(cols.SER, LOW);
      }
      cols.incrementSR();
    }
    for(int8_t j = 7; j >= 0; j--){ // writes -GND (81) (OFF)
      if((j+c_offset == c) && !state){
        digitalWrite(cols.SER, HIGH);
      }
      else{
        digitalWrite(cols.SER, LOW);
      }
      cols.incrementSR();
    }
  }
  cols.incrementR();
  cols.setIdle();
}

void toggleRow(short r, short c, short state){
  rows.disable();
  rows.clear();
  int8_t board = c/cols.numOnBoard;
  for(int8_t b = board; b>=0; b--){
    for(int8_t i = 1; i >=0; i--){
      short r_offset = i*8;
      for(int8_t j = 7; j >= 0; j--){ // writes +POS (83) (OFF)
        if((j+r_offset == r) && !state && (b == board)){
          digitalWrite(rows.SER, HIGH);
        }
        else{
          digitalWrite(rows.SER, LOW);
        }
        rows.incrementSR();
      }
      for(int8_t j = 7; j >= 0; j--){ // writes -GND (81) (ON)
        if((j+r_offset == r) && state && (b == board)){
          digitalWrite(rows.SER, HIGH);
        }
        else{
          digitalWrite(rows.SER, LOW);
        }
        rows.incrementSR();
      }
    }
  }
  rows.incrementR();
  rows.setIdle();
}

void flashDisplay(){
  rows.enable();
  cols.enable();
  delayMicroseconds(FLASHTIME);
  cols.disable();
  rows.disable();
}

void writeDot(uint8_t r, uint8_t c, bool state){ // one dot at a time
  toggleRow(r, c, state);
  toggleColumn(c, state);
  flashDisplay();
}

/*
void writeDisplayFull(unsigned char array[NUMCOLS]){ // one col at a time
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

void writeDisplay(uint8_t * array, int x, int y){ // one dot at a time
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
*/

void setup() {
  Serial.begin(9600);
  pinMode(cols.SER, OUTPUT);
  pinMode(cols.OE, OUTPUT);
  pinMode(cols.RCLK, OUTPUT);
  pinMode(cols.SRCLK, OUTPUT);
  pinMode(cols.SRCLR, OUTPUT);
  cols.disable();
  cols.clear();
  pinMode(rows.SER, OUTPUT);
  pinMode(rows.OE, OUTPUT);
  pinMode(rows.RCLK, OUTPUT);
  pinMode(rows.SRCLK, OUTPUT);
  pinMode(rows.SRCLR, OUTPUT);
  rows.disable();
  rows.clear();
}


void loop() {
  
  for(uint8_t i = 0; i<16; i++){
    for(uint8_t j = 0; j<84; j++){
      writeDot(i,j,1);
      delay(5);
    } 
  }
  delay(5000);
  for(uint8_t i = 0; i<16; i++){
    for(uint8_t j = 0; j<84; j++){
      writeDot(i,j,0);
      delay(5);
    } 
  }
  delay(5000);

}
