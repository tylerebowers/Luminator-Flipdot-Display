struct shiftRegister{
  int serial;
  int outputEnable;
  int RCLK;
  int SRCLK;
  int clear;
} col, row;

void writeRegister(struct shiftRegister sr, int array[8]){
  digitalWrite(sr.outputEnable, HIGH);
  digitalWrite(sr.clear, LOW);
  digitalWrite(sr.clear, HIGH);
  for(int i = 0; i < 8; i++){
    digitalWrite(sr.serial, array[i]);
    digitalWrite(sr.SRCLK, HIGH);
    digitalWrite(sr.SRCLK, LOW);
    digitalWrite(sr.RCLK, HIGH);
    digitalWrite(sr.RCLK, LOW);
  }
  digitalWrite(sr.serial, LOW);
  digitalWrite(sr.outputEnable, LOW);
}

void clearRegister(struct shiftRegister sr){
  digitalWrite(sr.outputEnable, HIGH);
  digitalWrite(sr.clear, LOW);
  digitalWrite(sr.clear, HIGH);
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
  int coloff[8] = {0,0,0,0,0,0,0,1};
  int colon[8] = {1,1,1,1,0,0,1,0};
  int colt1[8] = {0,0,0,0,0,1,0,0};
  int colt2[8] = {1,1,1,1,1,0,0,0};

  writeRegister(col, coloff);
  delay(1000);
  writeRegister(col, colon);
  delay(1000);
  writeRegister(col, colt1);
  delay(1000);
  writeRegister(col, colt2);
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
