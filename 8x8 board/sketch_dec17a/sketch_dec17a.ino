#define NUMROWS 8
#define NUMCOLS 8


struct shiftRegister{
  int serial;
  int outputEnable;
  int RCLK;
  int SRCLK;
  int clear;
} col, row;

void writeRegisters(struct shiftRegister sr, int * array, int n){  //deprecated 
  digitalWrite(sr.clear, LOW); // clear
  digitalWrite(sr.clear, HIGH); // clear
  digitalWrite(sr.outputEnable, HIGH); // disable
  for(int i = n-1; i >= 0; i--){
    digitalWrite(sr.serial, array[i]);
    digitalWrite(sr.SRCLK, HIGH);
    digitalWrite(sr.SRCLK, LOW);
    digitalWrite(sr.RCLK, HIGH);
    digitalWrite(sr.RCLK, LOW);
  }
  digitalWrite(sr.serial, LOW);
  digitalWrite(sr.outputEnable, LOW); // enable
}

void clearRegister(struct shiftRegister sr){
  digitalWrite(sr.outputEnable, HIGH);
  digitalWrite(sr.clear, LOW);
  digitalWrite(sr.clear, HIGH);
  digitalWrite(sr.serial, LOW);
  digitalWrite(sr.outputEnable, LOW);
}


void inverseArray(int * array, int n){
  for(int i = 0; i < n; i++){
    if (array[i]){
      array[i] = 0;
    } else {
      array[i] = 1;
    }
  }
}

void writeArray(struct shiftRegister sr, int * array, int n, int state){
  digitalWrite(sr.clear, LOW); // clear
  digitalWrite(sr.clear, HIGH); // clear
  digitalWrite(sr.outputEnable, HIGH); // disable
  for(int i = n/8; i > 0; i--){
    for(int j = 1; j <= 8; j++){ // writes +POS (83)
      //Serial.print("i:");Serial.print(i);Serial.print(", j:");Serial.print(j);Serial.print(", array:");Serial.print(array[(8*i)-j]);Serial.print("\n");
      if(state == 1 && array[(8*i)-j] == 1){
        //Serial.print(" ^ +H WROTE\n");
        digitalWrite(sr.serial, HIGH);
      }
      else{
        digitalWrite(sr.serial, LOW);
        Serial.print(" ^ +L WROTE\n");
      }
      digitalWrite(sr.SRCLK, HIGH);
      digitalWrite(sr.SRCLK, LOW);
      digitalWrite(sr.RCLK, HIGH);
      digitalWrite(sr.RCLK, LOW);
    }
    for(int k = 1; k <= 8; k++){ // writes -GND (81)
      //Serial.print("i:");Serial.print(i);Serial.print("k:");Serial.print(k);Serial.print("array:");Serial.print(array[(8*i)-k]);Serial.print("\n");
      if(state == 0 && array[(8*i)-k] == 0){
        //Serial.print(" ^ GH WROTE\n");
        digitalWrite(sr.serial, HIGH);
      }
      else{
        digitalWrite(sr.serial, LOW);
        Serial.print(" ^ GL WROTE\n");
      }
      digitalWrite(sr.SRCLK, HIGH);
      digitalWrite(sr.SRCLK, LOW);
      digitalWrite(sr.RCLK, HIGH);
      digitalWrite(sr.RCLK, LOW);
    }
  }
  digitalWrite(sr.serial, LOW);
  digitalWrite(sr.outputEnable, LOW); // enable
}
/*
void writeDisplay(int array[8][8]){
  int rowon[16];
  int rowoff[16];
  int done[16] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
  int invdone[16] = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};
  int colon[16];
  int coloff[16];
  
  for(int j = 0; j < 8; j++){
      rowon[j] = 0;
      rowoff[j+8] = 1;
      colon[j+8] = 1;
      coloff[j] = 0;
  }
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      rowoff[j] = array[j][i];
      if (array[j][i]){
        rowon[j+8] = 0;
      } else {
        rowon[j+8] = 1;
      }
      colon[j] = 0;
      coloff[j+8] = 1;
    }
    colon[i] = 1;
    coloff[i+8] = 0;

    writeRegister(col, colon);
    writeRegister(row, rowon);
    delay(100);
    writeRegister(col, coloff);
    writeRegister(row, rowoff);
    delay(100);
    writeRegister(col, done);
    writeRegister(row, done);

    Serial.print("\nCOLUMN: ");
    Serial.print(i);    
    Serial.print(" WROTE:\nCOLON: ");
    for(int i = 0; i < 16; i++){Serial.print(colon[i]);}
    Serial.print("\nROWON: ");
    for(int i = 0; i < 16; i++){Serial.print(rowon[i]);}
    Serial.print("\nCOLOFF:");
    for(int i = 0; i < 16; i++){Serial.print(coloff[i]);}
    Serial.print("\nROWOFF:");
    for(int i = 0; i < 16; i++){Serial.print(rowoff[i]);}
    Serial.print("\nDONE:  ");
    for(int i = 0; i < 16; i++){Serial.print(done[i]);}
    Serial.print("\nEND WRITE");
  }
}
*/
void setup() {
  Serial.begin(9600);
  col.serial = 2;
  col.RCLK = 3;
  col.SRCLK = 4;
  col.outputEnable = 5;
  col.clear = 6;
  pinMode(col.serial, OUTPUT);
  pinMode(col.outputEnable, OUTPUT);
  pinMode(col.RCLK, OUTPUT);
  pinMode(col.SRCLK, OUTPUT);
  pinMode(col.clear, OUTPUT);
  digitalWrite(col.outputEnable, LOW);
  digitalWrite(col.clear, HIGH);
  digitalWrite(col.clear, LOW);
  row.serial = 7;
  row.RCLK = 8;
  row.SRCLK = 9;
  row.outputEnable = 10;
  row.clear = 11;
  pinMode(row.serial, OUTPUT);
  pinMode(row.outputEnable, OUTPUT);
  pinMode(row.RCLK, OUTPUT);
  pinMode(row.SRCLK, OUTPUT);
  pinMode(row.clear, OUTPUT);
  digitalWrite(row.outputEnable, HIGH); //not enabled
  digitalWrite(row.clear, LOW); //clear
  digitalWrite(row.clear, HIGH); //normal
  //int testarr[8] = {1,0,0,0,0,0,0,0};
  //writeArray(col, testarr, 8, 1);// REMOVE 
}

void loop() {
  delay(1000);
  /*
  int testarr[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  int testarr2[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  writeRegister(col, testarr2);
  writeRegister(row, testarr2);
  delay(1000);
  writeRegister(col, testarr);
  writeRegister(row, testarr);
  delay(1000);
  
  int coloff[8] = {0,0,0,0,0,0,0,1};
  int colon[8] = {1,1,1,1,0,0,1,0};
  int colt1[8] = {0,0,0,0,0,1,0,0};
  int colt2[8] = {1,1,1,1,1,0,0,0};

  writeRegister(row, coloff, NUMROWS);
  delay(1000);
  writeRegister(row, colon, NUMROWS);
  delay(1000);
  writeRegister(row, colt1, NUMROWS);
  delay(1000);
  writeRegister(row, colt2, NUMROWS);
  delay(1000);
  /*
  int circle[8][8] = {{0,0,0,0,0,0,0,0},{0,0,0,1,1,0,0,0},{0,0,1,0,0,1,0,0},{0,1,0,0,0,0,1,0},{0,1,0,0,0,0,1,0},{0,0,1,0,0,1,0,0},{0,0,0,1,1,0,0,0},{0,0,0,0,0,0,0,0}};
  int blank[8][8] = {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
  int full[8][8] = {{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}};
  // put your main code here, to run repeatedly:
  delay(1000);
  writeDisplay(blank);
  delay(1000);

  delay(1000);
  writeDisplay(circle);
  delay(1000);
  writeDisplay(full);
  delay(1000);
  */

}
