#include <string.h>
#include <WiFi.h>
#include <RTClib.h>
//#include <time.h> 
#include <Wire.h>
#include <LinkedList.h>
//#include "libraries.h"

#define FLASHTIME 200   //in microseconds
#define TIMEZONE "EST5EDT,M3.2.0,M11.1.0"
#define NTP_SERVER "pool.ntp.org"
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

RTC_DS3231 RTC;
DateTime RTCnow;
uint8_t shownMinute = 61;
time_t now;

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
  void writeCol(uint16_t col, uint8_t x, uint8_t yS, uint8_t yF){ 
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


struct UI{


  uint16_t space[1] = {0};

  uint16_t ascii57[95][5] = {	//7r x 5c
    {0,0,0,0,0},      //" "
    {0,0,95,0,0},		  //!
    {0,3,0,3,0},		  //"
    {20,62,20,62,20},	//#
    {70,73,127,73,49},//$
    {6,102,28,51,48},	//%
    {54,73,81,38,88},	//&
    {0,0,3,0,0},		  //'
    {0,62,65,0,0},		//(
    {0,0,65,62,0},		//)
    {0,5,2,5,0},		  //*
    {0,8,28,8,0},	  	//+
    {0,64,32,0,0},		//,
    {0,8,8,8,0},		  //-
    {0,0,64,0,0},		  //.
    {0,96,28,3,0},		///
    {62,65,65,65,62},	//0
    {0,66,127,64,0},	//1
    {66,97,81,73,70},	//2
    {73,73,73,73,54},	//3
    {15,8,8,127,8},		//4
    {79,73,73,73,49},	//5
    {62,73,73,73,48},	//6
    {1,1,97,25,7},		//7
    {54,73,73,73,54},	//8
    {6,73,73,73,62},	//9
    {0,0,36,0,0},		  //:
    {0,64,36,0,0},		//,
    {0,8,20,34,0},		//<
    {0,20,20,20,0},		//=
    {0,34,20,8,0},		//>
    {2,1,81,9,6},		  //?
    {62,93,85,77,62},	//@
    {126,9,9,9,126},	//A
    {127,73,73,73,54},//B
    {62,65,65,65,34},	//C
    {127,65,65,65,62},//D
    {127,73,73,73,73},//E
    {127,9,9,9,9},		//F
    {62,65,65,73,56},	//G
    {127,8,8,8,127},	//H
    {0,65,127,65,0},	//I
    {32,64,65,65,63},	//J
    {127,8,20,34,65},	//K
    {127,64,64,64,64},//L
    {127,2,4,2,127},	//M
    {127,4,8,16,127},	//N
    {62,65,65,65,62},	//O
    {127,9,9,9,6},		//P
    {62,65,65,33,94},	//Q
    {127,9,9,9,118},	//R
    {70,73,73,73,49},	//S
    {1,1,127,1,1},		//T
    {63,64,64,64,63},	//U
    {7,56,64,56,7},		//V
    {127,32,28,32,127},	//W
    {99,20,8,20,99},	//X
    {3,4,120,4,3},		//Y
    {97,81,73,69,67},	//Z
    {0,127,65,0,0},		//[
    {0,3,28,96,0},		// "\"
    {0,0,65,127,0},		//]
    {0,2,1,2,0},		  //^
    {0,64,64,64,0},		//_
    {0,0,1,2,0},		  //`
    {32,84,84,84,120},//a
    {127,72,72,72,48},//b
    {48,72,72,72,0},	//c
    {48,72,72,72,127},//d
    {56,84,84,84,24},	//e
    {0,126,5,1,0},		//f
    {0,76,82,82,44},	//g
    {127,8,8,112,0},	//h
    {0,0,122,0,0},		//i
    {0,32,64,64,58},	//j
    {127,16,108,0,0},	//k
    {0,63,64,0,0},		//l
    {112,8,112,8,112},//m
    {0,112,8,112,0},	//n
    {48,72,72,48,0},	//o
    {0,124,36,24,0},	//p
    {0,28,20,124,64},	//q
    {0,112,8,8,0},		//r
    {0,88,84,52,0},		//s
    {0,4,62,68,0},		//t
    {0,56,64,64,56},	//u
    {0,56,64,56,0},		//v
    {56,64,56,64,56},	//w
    {68,40,16,40,68},	//x
    {0,92,80,124,0},	//y
    {0,98,82,74,70},	//z
    {0,8,54,65,0},		//{
    {0,0,127,0,0},		//|
    {0,65,54,8,0},		//}
    {8,4,8,16,8},		  //~
  };

  uint16_t largeTime[13][10] = {		                            //14r x 10c
    {4092,8190,14343,12291,12291,12291,12291,14343,8190,4092},	//0
    {0,12300,12302,12303,16383,16383,12288,12288,12288,0},	  	//1
    {0,14342,15367,15875,14083,13187,12739,12543,12414,0},	  	//2
    {0,6150,14343,12291,12483,12483,12483,14535,8190,3900},	  	//3
    {127,255,192,192,192,192,192,16383,16383,192},			        //4
    {12415,12415,12387,12387,12387,12387,12387,14563,8131,3971},//5
    {4094,8191,14563,12387,12387,12387,12387,14563,8135,3974},	//6
    {3,3,3,3,14339,16131,1987,243,63,15},					             	//7
    {3868,8190,14823,12483,12483,12483,12483,14823,8190,3900},	//8
    {60,6270,14567,12483,12483,12483,12483,14567,8190,4092},	  //9
    {0,0,0,0,1560,1560,0,0,0,0},						                		//:
    {0,3584,2560,1536,0,3584,512,3584,512,3584},			        	//am
    {0,15872,2560,3584,0,3584,512,3584,512,3584},		        		//pm
  };

  void writeLine(char * text, bool line = 0){
    uint8_t c = 0;
    uint8_t yLow = line*8;
    uint8_t yHigh = line*8+8;
    for(int16_t i = 0; i<strlen(text) && i<18; i++){
      int8_t charI = text[i]-32;
      if((charI >= 0) && (charI < 95)){
        display.write(ascii57[charI], 5, c, yLow, yHigh);
        c+=5;
        display.write(space, 1, c++, yLow, yHigh);
      }
    }
  }

  void writeTime(uint8_t hour, uint8_t minute, uint8_t c = 0, bool alignLeft = true, bool mt = false){
    uint8_t t[6] = {hour / 10, hour % 10, 10, minute / 10, minute % 10, 11+(hour>=12)};
    int8_t s = 0;
    if(!mt){
      if(hour > 12){t[0]-=1;t[1]-=2;} 
      else if(hour == 0){t[0]=1;t[1]+=2;}
      if(t[0] == 0){s = 1;}
    }
    if(alignLeft){
      c+=10;
      for(int8_t d = 5-mt; d >= s; d--){
        display.write(largeTime[t[d]], 10, display.numCols-c, 1, 15);
        c+=10;
        display.write(space, 1, display.numCols-c++, 0, 16);
      }
    } else {
      for(int8_t d = s; d <= 5-mt; d++){
        display.write(largeTime[t[d]], 10, c, 1, 15);
        c+=10;
        display.write(space, 1, c++, 0, 16);
      }
    }
  }

  void clearLine(bool line = 0, bool largeLetters = 0){
    Serial.println("TODO");
  }

} ui;

/*
struct tm localTime;
void syncRTC(){
  if(WiFi.status() == WL_CONNECTED && getLocalTime(&localTime)){
    RTC.adjust(DateTime(localTime.tm_year+1900, localTime.tm_mon+1, localTime.tm_mday, localTime.tm_hour, localTime.tm_min, localTime.tm_sec));    //update RTC
    Serial.println(&localTime, "Synced RTC: %A, %B %d %Y %H:%M:%S");
  } else if (WiFi.status() != WL_CONNECTED) {
    Serial.println("ERROR: wifi is not connected OR there was a problem getting new time!");
  }
}
*/

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

  /*
  //Connect to wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  for(int i = 0; i < 20; i++){
    if(WiFi.status() == WL_CONNECTED){
      Serial.print("WiFi Connection Established! (IP address: ");Serial.print(WiFi.localIP());Serial.println(")");
      break;
    } else {
      Serial.printf("Attempting WiFi connection (%d)\n",i+1);
    }
    delay(1000);
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi Connection Error!");
  }

  //configure and sync time
  configTzTime(TIMEZONE, NTP_SERVER);
  //syncRTC();
  */
}


void loop() {
  
  ui.writeLine("Hello World");
  serialPrintDisplay();
  Serial.println();
  delay(5000);

  ui.writeLine("ABCDEFGHIJKLMNOPQR", 0);
  ui.writeLine("STUVWXYZ1234567890", 1);
  serialPrintDisplay();
  Serial.println();
  delay(5000);


  display.allOff();
  ui.writeTime(13, 24);
  serialPrintDisplay();
  Serial.println();
  delay(5000);
  
  /*
  struct tm localTime;

  if(!getLocalTime(&localTime)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&localTime, "%A, %B %d %Y %H:%M:%S");
  
  ui.writeTime(localTime.tm_hour, localTime.tm_min, 0);

  serialPrintDisplay();

  //syncRTC();
  //RTCnow = RTC.now();
  //Serial.printf("TIME: %d:%d\n",RTCnow.hour(), RTCnow.minute());
  delay(10000);

  while(true){  //wait till next minute
    RTCnow = RTC.now();
    if(RTCnow.minute() != shownMinute){    //update clock
      if(RTCnow.hour() == 12 && RTCnow.minute() == 0){
        syncRTC();
      } 
      RTCnow = RTC.now();
      Serial.printf("TIME: %d:%d\n",RTCnow.hour(), RTCnow.minute());
      writeTime(RTCnow.hour(), RTCnow.minute());
      shownMinute = RTCnow.minute();
      break;
    }
    delay(500);
  }   
  delay(30000);
  */
}
