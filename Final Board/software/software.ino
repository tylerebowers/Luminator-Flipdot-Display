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

  void writeDot(uint8_t r, uint8_t c, bool state){ // one dot at a time
    toggleRow(r, c, state);
    toggleColumn(c, state);
    flash();
    shown[c] = (shown[c] & (~(1 << r))) | (state << r);
  }

  void writeCol(uint16_t col, uint8_t c){
    for(short r = 0; r < numRows; r++){ // do each row 
      bool newDot = ((col >> r) & 1);
      if(newDot != ((shown[c] >> r) & 1)){
        writeDot(r, c, newDot);
      }
    }
  }

  void write(uint16_t * array, uint8_t x = 0, uint8_t y = 0){ // one dot at a time
    for(short c = x; c < numCols; c++){ // For each column
      //Serial.printf("current:%d, changeto:%d\n",shown[c],array[c]);
      for(short r = y; r < numRows; r++){ // do each row 
        bool newDot = ((array[c] >> r) & 1);
        if(newDot != ((shown[c] >> r) & 1)){
          writeDot(r, c, newDot);
        }
      }
    }
  }

  void allOff(short delayTime = 5){
    for(uint8_t i = 0; i<numRows; i++){
      for(uint8_t j = 0; j<numCols; j++){
        writeDot(i,j,0);
        delay(delayTime);
      } 
    }
    for(uint8_t i = 0; i<numCols; i++){
      shown[i] = 0;
    }
  }

  void allOn(short delayTime = 5){
    for(uint8_t i = 0; i<numRows; i++){
      for(uint8_t j = 0; j<numCols; j++){
        writeDot(i,j,1);
        delay(delayTime);
      } 
    }
    for(uint8_t i = 0; i<numCols; i++){
      shown[i] = 65535;
    }
  }

} display;

struct Cursor{
  void writeText(String toWrite){

  }

} cursor;

/*
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

void writeTime(uint8_t hour, uint8_t minute, uint8_t c = 0, bool mt = false){
    uint8_t t[6] = {hour / 10, hour % 10, 10, minute / 10, minute % 10, 11+(hour>=12)};
    uint8_t d = 0;
    if(!mt){
      if(hour > 12){t[0]-=1;t[1]-=2;} 
      else if(hour == 0){t[0]=1;t[1]+=2;}
      if(t[0] == 0){d = 1;}
    }
    for(d; d < 6-mt; d++){
        for(uint8_t i = 0; i < 10; i++){
            if(largeTime[t[d]][i] != 0){
              display.writeCol(largeTime[t[d]][i], c++);
            }
        }
        display.writeCol(0, c++);
    }

    
}


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
    for(uint16_t c = 0; c<112; c++){
      if((array[c] >> r) & 1){
        Serial.print('#');
      } else {
        Serial.print(' ');
      }
    }
    Serial.println();
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
        display.write(newDisplay);
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

  ui.writeText("Hello World");
  
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
