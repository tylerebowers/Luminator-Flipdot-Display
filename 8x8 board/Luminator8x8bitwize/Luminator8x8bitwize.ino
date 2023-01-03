//this program opperates based on matrix values
//it is memory inefficient the bitwize version uses ~8x less ram.

#define NUMROWS 8
#define NUMCOLS 8
#define FLASHTIME 1   //in milliseconds
#define DEBUG 1

byte alphabet[][5] = {
	  {0,0,0,0,0},
	  {0,56,125,56,0},// !
	  {80,96,0,80,96},//"
	  {20,127,20,127,20},//#
	  {18,42,127,42,36},//$
	  {98,100,8,19,35},//%
	  {54,73,85,34,5},//&
	  {0,80,96,0,0},//'(7)
	  {0,28,34,65,0},//(
	  {0,65,34,28,0},//)
	  {20,8,62,8,20},//*
	  {8,8,62,8,8},//+
	  {0,5,6,0,0},//,(12)
	  {8,8,8,8,8},//-
	  {0,3,3,0,0},//.(14)
	  {2,4,8,16,32},// /
	  {62,69,73,81,62},//0
	  {17,33,127,1,1},//1
	  {33,67,69,73,49},//2
	  {34,65,73,73,54},//3
	  {12,20,36,127,4},//4
	  {114,81,81,81,78},//5
	  {62,73,73,73,38},//6
	  {64,71,72,80,96},//7
	  {54,73,73,73,54},//8
	  {50,73,73,73,62},//9
	  {0,54,54,0,0},//:(26)
	  {0,53,54,0,0},//;(27)
	  {8,20,34,65,0},//<
	  {20,20,20,20,20},//=
	  {0,65,34,20,8},//>
	  {32,64,69,72,48},//?
	  {62,73,87,85,62},//@
	  {31, 36, 68, 36, 31},    //A
	  {127, 73, 73, 73, 54},   //B
	  {62, 65, 65, 65, 34},    //C
	  {127, 65, 65, 34, 28},   //D
	  {127, 73, 73, 65, 65},   //E
	  {127, 72, 72, 72, 64},   //F
	  {62, 65, 65, 69, 38},    //G
	  {127, 8, 8, 8, 127},     //H
	  {0, 65, 127, 65, 0},     //I
	  {2, 1, 1, 1, 126},       //J
	  {127, 8, 20, 34, 65},    //K
	  {127, 1, 1, 1, 1},       //L
	  {127, 32, 16, 32, 127},  //M
	  {127, 32, 16, 8, 127},   //N
	  {62, 65, 65, 65, 62},    //O
	  {127, 72, 72, 72, 48},   //P
	  {62, 65, 69, 66, 61},    //Q
	  {127, 72, 76, 74, 49},	//R
	  {50, 73, 73, 73, 38},		//S
	  {64, 64, 127, 64, 64},	//T
	  {126, 1, 1, 1, 126},		//U
	  {124, 2, 1, 2, 124},		//V
	  {126, 1, 6, 1, 126},		//W
	  {99, 20, 8, 20, 99},		//X
	  {96, 16, 15, 16, 96},		//Y
	  {67, 69, 73, 81, 97},		//Z
	  {0,127,65,65,0},//[
	  {32,16,8,4,2},//
	  {0,65,65,127,0},
	  {16,32,64,32,16},//^
	  {1,1,1,1,1},//_
	  {0,64,32,16,0},//`
	  {2,21,21,14,1},//a
	  {64,126,9,17,14},//b
	  {14,17,17,17,10},//c
	  {14,17,74,127,1},//d
	  {14,21,21,21,9},//e
	  {1,9,63,72,32},//f
	  {9,21,21,21,30},//g
	  {127,8,8,8,7},//h
	  {0,0,46,1,1},//i
	  {2,1,1,1,94},//j
	  {1,127,4,10,17},//k
	  {0,65,127,1,0},//l
	  {31,16,14,16,31},//m
	  {31,8,16,16,15},//n
	  {14,17,17,17,14},//o
	  {127,20,20,20,8},//p
	  {8,20,20,31,1},//q
	  {31,8,16,16,8},//r
	  {9,21,21,21,18},//s
	  {16,16,126,17,18},//t
	  {30,1,1,30,1},//u
	  {28,2,1,2,28},//v
	  {30,1,6,1,30},//w
	  {17,10,4,10,17},//x
	  {16,9,6,8,16},//y
	  {17,19,21,25,17},//z
	  {8,54,65,65,0},//{
	  {0,0,127,0,0},//|(92)
	  {0,65,65,54,8},//}
	  {32,64,32,16,32},//~
	};


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
void writeRowBits(unsigned short bitwize, short state){ // dont use (this version is bad for the drivers)
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

void writeDisplayFull(short array[NUMCOLS]){ // one col at a time
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


unsigned short circle[NUMCOLS] = {0,24,36,66,66,36,24,0};  // each number representa a column for 8 rows it is 1 byte (char), 16 rows 2 bytes (short), etc
unsigned short blank[NUMCOLS] = {0,0,0,0,0,0,0,0};
unsigned short full[NUMCOLS] = {255,255,255,255,255,255,255,255};
unsigned short north[NUMCOLS] = {128, 128, 190, 144, 136, 190, 128, 128};
unsigned short south[NUMCOLS] = {1, 1, 117, 85, 85, 93, 1, 1};
unsigned short east[NUMCOLS] = {0, 0, 124, 84, 84, 0, 0, 255};
unsigned short west[NUMCOLS] = {255, 0, 56, 8, 56, 8, 56, 0};
unsigned short confetti[NUMCOLS] = {170, 85, 170, 85, 170, 85, 170, 85};
unsigned short animation1[NUMCOLS] = {0, 126, 129, 129, 129, 129, 126, 0};
unsigned short animation2[NUMCOLS] = {0, 0, 126, 129, 129, 126, 0, 0};
unsigned short animation3[NUMCOLS] = {0, 0, 0, 255, 255, 0, 0, 0};
unsigned short animation4[NUMCOLS] = {60, 66, 129, 129, 129, 129, 66, 60};


void loop() {
  delay(3000);
  writeDisplayFull(blank);
  delay(500);
  writeDisplayFull(north);
  delay(1000);
  writeDisplayFull(south);
  delay(1000);
  writeDisplayFull(east);
  delay(1000);
  writeDisplayFull(west);
  delay(1000);
  writeDisplayFull(confetti);
  delay(1000);
  writeDisplayFull(blank);
  delay(1000);
  for(int i = 0; i < 20; i++){
    writeDisplayFull(animation1);
    delay(100);
    writeDisplayFull(animation2);
    delay(100);
    writeDisplayFull(animation3);
    delay(100);
    writeDisplayFull(animation4);
    delay(100);
    writeDisplayFull(animation3);
    delay(100);
    writeDisplayFull(animation2);
    delay(100);
  }
  for(int i = 0; i < 95; i++){
    writeDisplay(alphabet[i], 5, 7);
    delay(500);
    writeDisplayFull(blank);
    delay(100);
  }
  /*
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      //delay(500);
      writeRowSingle(i, 1);
      writeColumnSingle(j, 1);
      flashDisplay(); 
    }
  }
  //delay(500);
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      //delay(500);
      writeRowSingle(i, 0);
      writeColumnSingle(j, 0);
      flashDisplay(); 
    }
  }
  //delay(500);
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

}
