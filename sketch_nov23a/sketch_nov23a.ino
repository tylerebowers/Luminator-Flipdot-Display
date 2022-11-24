struct shiftRegister{
  int serial;
  int outputEnable;
  int RCLK;
  int SRCLK;
  int clear;
} col, row;

void writeRegister(struct shiftRegister sr, int array[16]){
  digitalWrite(sr.outputEnable, HIGH);
  digitalWrite(sr.clear, LOW);
  digitalWrite(sr.clear, HIGH);
  for(int i = 0; i < 16; i++){
    digitalWrite(sr.serial, array[i]);
    digitalWrite(sr.SRCLK, HIGH);
    digitalWrite(sr.SRCLK, LOW);
    digitalWrite(sr.RCLK, HIGH);
    digitalWrite(sr.RCLK, LOW);
  }
  digitalWrite(sr.serial, LOW);
  digitalWrite(sr.outputEnable, LOW);
}


void inverseArray(int *array[8]){
  for(int i = 0; i < 8; i++){
    if (array[i]){
      array[i] = 0;
    } else {
      array[i] = 1;
    }
  }
}

void writeDisplay(int array[8][8]){
  int rowon[16];
  int rowoff[16];
  int done[16] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
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
  }
}
void setup() {
  Serial.begin(9600);
  col.serial = 2;
  col.outputEnable = 3;
  col.RCLK = 4;
  col.SRCLK = 5;
  col.clear = 6;
  row.serial = 7;
  row.outputEnable = 8;
  row.RCLK = 9;
  row.SRCLK = 10;
  row.clear = 11;
  pinMode(col.serial, OUTPUT);
  pinMode(col.outputEnable, OUTPUT);
  pinMode(col.RCLK, OUTPUT);
  pinMode(col.SRCLK, OUTPUT);
  pinMode(col.clear, OUTPUT);
  pinMode(row.serial, OUTPUT);
  pinMode(row.outputEnable, OUTPUT);
  pinMode(row.RCLK, OUTPUT);
  pinMode(row.SRCLK, OUTPUT);
  pinMode(row.clear, OUTPUT);
  digitalWrite(col.outputEnable, LOW);
  digitalWrite(col.clear, HIGH);
  digitalWrite(row.outputEnable, LOW);
  digitalWrite(row.clear, HIGH);
}

void loop() {
  /*
  int testarr[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  int testarr2[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  writeRegister(col, testarr2);
  writeRegister(row, testarr2);
  delay(1000);
  writeRegister(col, testarr);
  writeRegister(row, testarr);
  delay(1000);
  */
  int circle[8][8] = {{0,0,0,0,0,0,0,0},{0,0,0,1,1,0,0,0},{0,0,1,0,0,1,0,0},{0,1,0,0,0,0,1,0},{0,1,0,0,0,0,1,0},{0,0,1,0,0,1,0,0},{0,0,0,1,1,0,0,0},{0,0,0,0,0,0,0,0}};
  int blank[8][8] = {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
  int full[8][8] = {{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}};
  // put your main code here, to run repeatedly:
  writeDisplay(blank);
  delay(1000);
  writeDisplay(circle);
  delay(1000);
  writeDisplay(full);
  delay(1000);

}
