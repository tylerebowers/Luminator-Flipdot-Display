// FLIP-DOT TEST
// -------------
// Roger Meier
// August 2018
//
// https://hardware.the-meiers.org


/*
 
 Circuit:
 EN: pin 6
 CSB: pin 10
 MOSI: pin 11
 MISO: pin 12
 SCK: pin 13
 
 */

// The NCV7719 communicate using SPI, so include the library:
// be sure to include the "../lib/FlipDot_24x16" folder via "Sketch/Include Library/Add.ZIP Library..."
// Install the Adafruit_GFX library and its dependencies via the Library manager
#include <FlipDot_24x16.h>

FlipDot_24x16 flipdot;

// initialize variables

void setup() {
  //Serial.begin(9600);
  flipdot.setPixelDelay(20);
  flipdot.begin();
//  flipdot.fillScreen(FLIPDOT_YELLOW);
//  flipdot.display();
//  delay(1000);
//  flipdot.fillScreen(FLIPDOT_BLACK);
//  flipdot.display();
  delay(3000);
}


void loop() {

  // Flash Screen
  flipdot.fillScreen(FLIPDOT_YELLOW);
  flipdot.display();
  delay(1000);
  flipdot.fillScreen(FLIPDOT_BLACK);
  flipdot.display();
  delay(1000);

  // Hello World
  flipdot.setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
  flipdot.setFont();
  flipdot.setCursor(0,0);
  flipdot.print("Hello...");
  flipdot.display();
  delay(1000);
  flipdot.setCursor(0,0);
  flipdot.print("World!!!");
  flipdot.display();
  delay(1000);
  flipdot.fillScreen(FLIPDOT_BLACK);
  // Rectangles
  for(int j=0; j<4; j++) {
    for(int i=0; i<8; i++){
      flipdot.drawRect(i,i,24-2*i,16-2*i,FLIPDOT_YELLOW);
      flipdot.display();
      flipdot.drawRect(i,i,24-2*i,16-2*i,FLIPDOT_BLACK);
      delay(100);  
    }
  }
  flipdot.display();

  // Checkerboard
  for(int x=0; x<24; x++) {
    for(int y=0; y<16; y++) {
      flipdot.drawPixel(x,y,(x+y) % 2);
    }
  }
  flipdot.display();
  delay(1000);
  flipdot.fillScreen(FLIPDOT_BLACK);
  for(int x=0; x<24; x++) {
    for(int y=0; y<16; y++) {
      flipdot.drawPixel(x,y,(x+y+1) % 2);
    }
  }
  flipdot.display();
  delay(1000);


  // Flood Screen
  for(int x=0; x<24; x++) {
    for(int y=0; y<16; y++) {
      flipdot.drawPixel(x,y,1);
      flipdot.display();
      delay(10);
   }
  }
  for(int x=0; x<24; x++) {
    for(int y=0; y<16; y++) {
      flipdot.drawPixel(x,y,0);
      flipdot.display();
      delay(10);
   }
  }

  // Bouncing Ball
  int r = 3; // radius 
  for(int x=0; x<=23-2*r; x++){
    flipdot.fillCircle(x+r,r+x*15/23,r,FLIPDOT_YELLOW);
    flipdot.display();
    flipdot.fillScreen(FLIPDOT_BLACK);
    delay(100);
  }
  for(int x=23-2*r-1; x>1; x--){
    flipdot.fillCircle(x+r,r+x*15/23,r,FLIPDOT_YELLOW);
    flipdot.display();
    flipdot.fillScreen(FLIPDOT_BLACK);
    delay(100);
  }
  for(int x=0; x<=23-2*r; x++){
    flipdot.fillCircle(x+r,r+x*15/23,r,FLIPDOT_YELLOW);
    flipdot.display();
    flipdot.fillScreen(FLIPDOT_BLACK);
    delay(100);
  }

  // Alphabet
  flipdot.setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
  int col = 0;
  for (char c = '0'; c <= 'z'; c++) {
    flipdot.fillScreen(FLIPDOT_BLACK);
    flipdot.setCursor(col % 19, col % 11);
    col++;
    flipdot.write(c);
    flipdot.display();
    delay(200);
  }
  flipdot.fillScreen(FLIPDOT_BLACK);


}
