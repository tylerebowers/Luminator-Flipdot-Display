#include <string.h>
#define FLASHTIME 200   //in microseconds

struct Display{
  uint8_t numRows = 16;
  uint8_t numCols = 112;    // numCols = numBoards * numColsPerBoard
  uint8_t numBoards = 4;
  uint8_t numColsPerBoard = 28;
  uint16_t shown[112];    // datatype is uint16_t because numRows = 16 
} display;

struct Rows{
  uint8_t SER = 32;
  uint8_t OE = 33;
  uint8_t RCLK = 25;
  uint8_t SRCLK = 26;
  uint8_t SRCLR = 27;
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

struct Columns{
  uint8_t SER = 23;
  uint8_t OE = 19;
  uint8_t RCLK = 18;
  uint8_t SRCLK = 17;
  uint8_t SRCLR = 16;
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

void toggleColumn(short c, bool state){
  cols.disable();
  cols.clear();
  c = c%display.numColsPerBoard;
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
  int8_t board = c/display.numColsPerBoard;
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

void flashDisplay(short delayTime = FLASHTIME){    // turn on rows and colums for FLASHTIME microseconds 
  rows.enable();
  cols.enable();
  delayMicroseconds(delayTime);
  cols.disable();
  rows.disable();
}

void writeDot(uint8_t r, uint8_t c, bool state){ // one dot at a time
  toggleRow(r, c, state);
  toggleColumn(c, state);
  flashDisplay();
  display.shown[c] = (display.shown[c] & (~(1 << r))) | (state << r);
}

void writeAllOff(short delayTime = 10){
  for(uint8_t i = 0; i<display.numRows; i++){
    for(uint8_t j = 0; j<display.numCols; j++){
      writeDot(i,j,0);
      delay(delayTime);
    } 
  }
  for(uint8_t i = 0; i<display.numCols; i++){
    display.shown[i] = 0;
  }
}

void writeAllOn(short delayTime = 10){
  for(uint8_t i = 0; i<display.numRows; i++){
    for(uint8_t j = 0; j<display.numCols; j++){
      writeDot(i,j,1);
      delay(delayTime);
    } 
  }
  for(uint8_t i = 0; i<display.numCols; i++){
    display.shown[i] = 65535;
  }
}

void writeDisplay(uint16_t * array, uint8_t x = 0, uint8_t y = 0){ // one dot at a time
  for(short c = x; c < display.numCols; c++){ // For each column
    //Serial.printf("current:%d, changeto:%d\n",display.shown[c],array[c]);
    for(short r = y; r < display.numRows; r++){ // do each row 
      if(((array[c] >> r) & 1) != ((display.shown[c] >> r) & 1)){
        writeDot(r, c, ((array[c] >> r) & 1)); //optimization needed here (dupe instruction)
      }
    }
  }
}


void userSerialConnection(){
  String userInput  = Serial.readStringUntil('\n');
  if (userInput.length() > 4){
    if(userInput.charAt(0) == '{'){
      uint16_t newDisplay[display.numCols] = {0};
      userInput = userInput.substring(userInput.indexOf('{')+1, userInput.indexOf('}'));
      char *ptr = strtok ((char *)userInput.c_str(), ",");         // get first token
      uint16_t loc = 0;
      while (ptr != NULL && loc < display.numCols){
        Serial.printf("%s,",ptr);
        newDisplay[loc++] = atoi(ptr);
        ptr = strtok(NULL, ",");
      }  
      Serial.println();
      writeDisplay(newDisplay);
    } else if(userInput.charAt(0) == '('){
      uint8_t dot[3] = {0};
      userInput = userInput.substring(userInput.indexOf('(')+1, userInput.indexOf(')'));
      char *ptr = strtok ((char *)userInput.c_str(), ",");         // get first token
      uint16_t loc = 0;
      while (ptr != NULL && loc < 3){
        Serial.printf("%s,",ptr);
        dot[loc++] = atoi(ptr);
        ptr = strtok(NULL, ",");
      } 
      Serial.println();
      if(loc == 3){writeDot(dot[0], dot[1], dot[2]);} 
    } else {
        Serial.println("Recieved incorrectly formatted string.");
    }
  }
}



void setup() {
  Serial.begin(115200);
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
  writeAllOff();
}


void loop() {
  userSerialConnection();
}
