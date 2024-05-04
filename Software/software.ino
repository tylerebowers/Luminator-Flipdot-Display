#include <string.h>

#define FLASHTIME 200   //in microseconds


struct Display{
  uint8_t numRows = 16;
  uint8_t numCols = 112;    // numCols = numBoards * numColsPerBoard
  uint8_t numBoards = 4;
  uint8_t numColsPerBoard = 28;
  uint16_t shown[112];    // datatype is uint16_t because numRows = 16 

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
    c = c%numColsPerBoard;
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
    int8_t board = c/numColsPerBoard;
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

  void flash(short delayTime = FLASHTIME){    // turn on rows and colums for FLASHTIME microseconds 
    rows.enable();
    cols.enable();
    delayMicroseconds(delayTime);
    cols.disable();
    rows.disable();
  }

  void writeDot(uint8_t c, uint8_t r, bool state){ // one dot at a time
    toggleRow(r, c, state);
    toggleColumn(c, state);
    flash();
    shown[c] = (shown[c] & (~(1 << r))) | (state << r);
  }
/*
  void writeByCol(uint16_t col, uint8_t x, uint8_t yS, uint8_t yF){ 
    for(uint8_t r = yS; r < min(numRows, yF); r++){ // do each row 
      bool newDot = ((col >> r-yS) & 1);
      if(newDot != ((shown[x] >> r) & 1)){
        writeDot(x, r, newDot);
      }
    }
  }
*/
  void write(uint16_t * array, uint8_t xLim, uint8_t x = 0, uint8_t y = 0, uint8_t yLim = 32){ 
    for(uint8_t c = x; c < numCols && c-x < xLim; c++){ // For each column
      for(uint8_t r = y; r < numRows && r-y < yLim; r++){ // do each row 
        bool newDot = ((array[c-x] >> r-y) & 1);
        if(newDot != ((shown[c] >> r) & 1)){
          writeDot(c, r, newDot);
        }
      }
    }
  }

  void allOff(bool full = true, uint16_t delayTime = 5){
    for(uint8_t j = 0; j<numRows; j++){
      for(uint8_t i = 0; i<numCols; i++){
        if(((shown[j] >> i) & 1) || full){
          writeDot(i,j,0);
          delay(delayTime);
        }
      } 
    }
    for(uint8_t i = 0; i<numCols; i++){
      shown[i] = 0;
    }
  }

  void allOn(bool full = true, uint16_t delayTime = 5){
    for(uint8_t j = 0; j<numRows; j++){
      for(uint8_t i = 0; i<numCols; i++){
        if(((shown[j] >> i) & 0) || full){
          writeDot(i,j,1);
          delay(delayTime);
        }
      } 
    }
    for(uint8_t i = 0; i<numCols; i++){
      shown[i] = 65535;
    }
  }

  void allInvert(uint16_t delayTime = 5){
    for(uint8_t j = 0; j<numRows; j++){
      for(uint8_t i = 0; i<numCols; i++){
        if(((shown[j] >> i) & 1)){
          writeDot(i,j,0);
        } else {
          writeDot(i,j,1);
        }
        delay(delayTime);
      } 
    }
    for(uint8_t i = 0; i<numCols; i++){
      shown[i] = 65535 - shown[i];
    }
  }

} display;


void serialPrintDisplay(uint16_t * array = display.shown){
  for(uint8_t r = 0; r<16; r++){
    Serial.print('|');
    for(uint16_t c = 0; c<112; c++){
      if((array[c] >> r) & 1){
        Serial.print('#');
      } else {
        Serial.print(' ');
      }
    }
    Serial.println('|');
  }
}

void userSerialConnection(){
  while(true){
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
        display.write(newDisplay, loc);
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
        if(loc == 3){display.writeDot(dot[0], dot[1], dot[2]);} 
      } else if (userInput == "print") {
        serialPrintDisplay();
      } else if (userInput == "exit") {
        break;  
      } else {
          Serial.println("Recieved incorrectly formatted string.");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(display.cols.SER, OUTPUT);
  pinMode(display.cols.OE, OUTPUT);
  pinMode(display.cols.RCLK, OUTPUT);
  pinMode(display.cols.SRCLK, OUTPUT);
  pinMode(display.cols.SRCLR, OUTPUT);
  display.cols.disable();
  display.cols.clear();
  pinMode(display.rows.SER, OUTPUT);
  pinMode(display.rows.OE, OUTPUT);
  pinMode(display.rows.RCLK, OUTPUT);
  pinMode(display.rows.SRCLK, OUTPUT);
  pinMode(display.rows.SRCLR, OUTPUT);
  display.rows.disable();
  display.rows.clear();
  display.allOff();
}


void loop() {
  userSerialConnection();
}
