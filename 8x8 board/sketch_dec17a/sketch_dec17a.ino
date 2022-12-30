#define NUMROWS 8
#define NUMCOLS 8
#define debug 1


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
  void SRIncrement(){
    digitalWrite(SRCLK, HIGH);
    digitalWrite(SRCLK, LOW);
  }
  void RIncrement(){
    digitalWrite(RCLK, HIGH);
    digitalWrite(RCLK, LOW);
  }
  void SetIdle(){
    digitalWrite(serial, LOW);
  }
} col, row;


void inverseArray(short * array, short n){
  for(short i = 0; i < n; i++){
    if (array[i]){
      array[i] = 0;
    } else {
      array[i] = 1;
    }
  }
}

void writeColumnArray(short * array, short n, short state){
  col.disable();
  col.clear();
  for(short i = n/8; i > 0; i--){
    for(short j = 1; j <= 8; j++){ // writes +POS (83) (ON)
      if(state == 1 && array[(8*i)-j] == 1){
        digitalWrite(col.serial, HIGH);
      }
      else{
        digitalWrite(col.serial, LOW);
      }
      col.SRIncrement();
      col.RIncrement();
    }
    for(short k = 1; k <= 8; k++){ // writes -GND (81) (OFF)
      if(state == 0 && array[(8*i)-k] == 1){
        digitalWrite(col.serial, HIGH);
      }
      else{
        digitalWrite(col.serial, LOW);
      }
      col.SRIncrement();
      col.RIncrement();
    }
  }
  col.SetIdle();
}

void writeRowArray(struct shiftRegister sr, short * array, short n, short state){
  row.disable();
  row.clear();
  for(short i = n/8; i > 0; i--){
    for(short j = 1; j <= 8; j++){ // writes +POS (83) (OFF)
      if(state == 0 && array[(8*i)-j] == 0){
        digitalWrite(row.serial, HIGH);
      }
      else{
        digitalWrite(row.serial, LOW);
      }
      row.SRIncrement();
      row.RIncrement();
    }
    for(short k = 1; k <= 8; k++){ // writes -GND (81) (ON)
      if(state == 1 && array[(8*i)-k] == 1){
        digitalWrite(row.serial, HIGH);
      }
      else{
        digitalWrite(row.serial, LOW);
      }
      row.SRIncrement();
      row.RIncrement();
    }
  }
  row.SetIdle();
}

void writeDisplay(short array[NUMROWS][NUMCOLS]){
  short colToWrite[NUMROWS] = {0};
  short rowToWrite[NUMCOLS] = {0};
  for(short i = 0; i < NUMCOLS; i++){
    for(short j = 0; j < NUMROWS; j++){
      array[j][i];
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
  short testarr[8] = {1,1,1,0,1,0,0,0};
  writeRowArray(row, testarr, 8, 0);// REMOVE 
}

void loop() {
  short circle[8][8] = {{0,0,0,0,0,0,0,0},{0,0,0,1,1,0,0,0},{0,0,1,0,0,1,0,0},{0,1,0,0,0,0,1,0},{0,1,0,0,0,0,1,0},{0,0,1,0,0,1,0,0},{0,0,0,1,1,0,0,0},{0,0,0,0,0,0,0,0}};
  short blank[8][8] = {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
  short full[8][8] = {{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}};
  /*
  delay(1000);
  writeDisplay(blank);
  delay(1000);
  writeDisplay(circle);
  delay(1000);
  writeDisplay(full);
  delay(1000);
  */

}
