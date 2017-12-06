/*
**  Made by Kamy Sheblid
**
**  I was having difficulty using the MEEPROMMER since I didn't have the 74HC595 shift register.
**  I used the TPIC6B595 Shift Register, which is basically the same as MEEPROMMER shift register
**  except that is the TPIC has OPEN DRAIN/COLLECTOR outputs.
**
*/


int REGCLK = A3; //For TPIC6B595 to latch data
//SCL and SDA are on A5 and A4 respectively

int CD = A0;
int OD = 10;
int WD = 11;

// To write to the EEPROM, you must either use writeEEPROM() to write a 
// single byte at a time, or use writePage() to write up to 64 bytes at a time.

void setup(){
  pinMode(REGCLK,OUTPUT);
  pinMode(SDA,OUTPUT);
  pinMode(SCL,OUTPUT);

  pinMode(CD,OUTPUT);
  pinMode(OD,OUTPUT);
  pinMode(WD,OUTPUT);

  delay(5);

  Serial.begin(115200);
  /*
  digitalWrite(OD,LOW);
  digitalWrite(WD,HIGH);
  digitalWrite(OD,HIGH);
  digitalWrite(CD,HIGH);  

  unsigned short add1 = 0x0;
  byte d1 = 0x1;

  pushAddress(add1);
  dataWriteBus();
  pushData(d1);

  digitalWrite(WD,LOW);
  digitalWrite(CD,LOW);

  digitalWrite(WD,HIGH);
  digitalWrite(CD,HIGH);

  toggleWait();
  */

/*
  unsigned long time =0;

  for(unsigned short b1 =0;b1<0xff;b1++){
  writeEEPROM(b1,b1);
  Serial.println(time);
  time++;
  }

  readPage();

  Serial.print( micros() );
  Serial.print("us\n");
  Serial.print( millis() );
  Serial.print("ms\n");
  */  

  softwareLock();
  
  delay(111);
  
  writeEEPROM(0x0,0x1a);
  
  delay(10);
  
  readPage();
}

void loop(){
}

void printByte(byte b1, unsigned short address){
  Serial.print("Byte at 0x");
  Serial.print(address,HEX);
  Serial.print(" is equal to 0x");
  Serial.println(b1,HEX);
}

void readPage(){ /* Read 256 bytes and output in pretty HEX format */
  for(unsigned long i=0;i<0x10;i++){
    Serial.print("\t 0x");
    Serial.print(i,HEX);
  }
  Serial.println();
  for(unsigned long i=0;i<0x100;i+=0x10){
    Serial.print("0x");
    Serial.print(i,HEX);
    for(unsigned long j=0;j<0x10;j++){
      Serial.print("\t 0x");
      Serial.print(readEEPROM(i+j),HEX);
    }
    Serial.println();
  }

}

void softwareLock(){
  dataWriteBus();
  
  digitalWrite(OD,LOW);
  
  pushAddress(0x5555);
  pushData(0xAA);
  
  digitalWrite(CD,LOW);
  digitalWrite(OD,HIGH);
  digitalWrite(WD,LOW);
  
  delayMicroseconds(1);
  
  digitalWrite(CD,HIGH);
  digitalWrite(WD,HIGH);
  digitalWrite(OD,LOW);
  
  pushAddress(0x2AAA);
  pushData(0x55);
  
  delayMicroseconds(1);
  
  digitalWrite(CD,LOW);
  digitalWrite(OD,HIGH);
  digitalWrite(WD,LOW);
  
  delayMicroseconds(1);
  
  digitalWrite(CD,HIGH);
  digitalWrite(WD,HIGH);
  digitalWrite(OD,LOW);
  
  pushAddress(0x5555);
  pushData(0xa0);
  
  delayMicroseconds(1);
  
  digitalWrite(CD,LOW);
  digitalWrite(OD,HIGH);
  digitalWrite(WD,LOW);
  
  delayMicroseconds(1);
  
  digitalWrite(CD,HIGH);
  digitalWrite(WD,HIGH);
  digitalWrite(OD,LOW);
  
  delay(25);
}

void softwareUnlock(){
  writeEEPROM(0x5555,0xAA);
  writeEEPROM(0x2AAA,0x55);
  writeEEPROM(0x5555,0x80);
  writeEEPROM(0x5555,0xAA);
  writeEEPROM(0x2AAA,0x55);
  writeEEPROM(0x5555,0x20);
  delay(25);
}

/*
  unsigned short ad = 0x0000;
 byte data = 0x82;
 
 Serial.print("Writing data 0x");
 Serial.print(data,HEX);
 Serial.print(" to address 0x");
 Serial.println(ad,HEX);
 writeEEPROM(ad,data);
 delayMicroseconds(1);
 
 delay(1);
 Serial.print("Reading address 0x");
 Serial.print(ad,HEX);
 Serial.print(" gives data 0x");
 Serial.println(readEEPROM(0),HEX );
 delay(10000);
 
 Serial.print("\n\n\n");
 */



byte readEEPROM(unsigned short address){
  pushAddress(address);
  dataReadBus();

  digitalWrite(CD,LOW);
  digitalWrite(OD,LOW);
  digitalWrite(WD,HIGH);

  byte dataIn = 0;

  for(int i=0;i<8;i++)
    dataIn+= bit(i) * (( bit(i) && digitalRead(i+2) ));

  return dataIn;
}

void writeEEPROM(unsigned short address,byte data){

  digitalWrite(OD,LOW);
  
  digitalWrite(WD,HIGH);
  digitalWrite(OD,HIGH); //For some reason, Output Disable must be brought high after Write Disable is brought High. It doesnt agree with the data sheet.
  digitalWrite(CD,HIGH);
  
  pushAddress(address);
  dataWriteBus();
  pushData(data);

  delayMicroseconds(1);

  digitalWrite(WD,LOW);
  digitalWrite(CD,LOW);

  delayMicroseconds(1);

  digitalWrite(WD,HIGH);

  toggleWaitAndNotify();
}

void writeEEPROMNoToggle(unsigned short address,byte data){

  digitalWrite(OD,LOW);
  
  digitalWrite(WD,HIGH);
  digitalWrite(OD,HIGH); //For some reason, Output Disable must be brought high after Write Disable is brought High. It doesnt agree with the data sheet.
  digitalWrite(CD,HIGH);
  
  pushAddress(address);
  dataWriteBus();
  pushData(data);

  delayMicroseconds(1);

  digitalWrite(WD,LOW);
  digitalWrite(CD,LOW);

  delayMicroseconds(1);

  digitalWrite(WD,HIGH);
  digitalWrite(CD,HIGH);
}

void toggleWait(){
  pinMode(8,INPUT_PULLUP);

  int toggle = 0;
  
  digitalWrite(CD,LOW);
  digitalWrite(OD,LOW);

  do{
    toggle = digitalRead(8);

    digitalWrite(CD,HIGH);
    digitalWrite(OD,HIGH);

    delayMicroseconds(10);

    digitalWrite(CD,LOW);
    digitalWrite(OD,LOW);

  }
  while( toggle!=digitalRead(8) ); 
}

void toggleWaitAndNotify(){
  pinMode(8,INPUT_PULLUP);

  int toggle = 0;
  int time = 0;

  digitalWrite(CD,LOW);
  digitalWrite(OD,LOW);

  do{
    toggle = digitalRead(8);

    digitalWrite(CD,HIGH);
    digitalWrite(OD,HIGH);

    delayMicroseconds(3);
    time++;

    digitalWrite(CD,LOW);
    digitalWrite(OD,LOW);

  }
  while( toggle!=digitalRead(8) ); 

  Serial.print("Write cycle took ");
  Serial.print(time*3);
  Serial.println("ms");

}


/* EEPROM CTRL BUS */


void ctrlWriteBus(){
  digitalWrite(WD,LOW);
  digitalWrite(OD,HIGH);
  digitalWrite(CD,LOW);
}

void ctrlWriteCLK(){
  digitalWrite(WD,HIGH);
  digitalWrite(CD,HIGH);
}

void ctrlReadBus(){
  digitalWrite(WD,HIGH);
  digitalWrite(OD,LOW);
  digitalWrite(CD,LOW);
}


/* SHIFT REGISTER ADDRESS INPUT */


void pushAddress(unsigned long address){
  address = ~address;
  digitalWrite(SCL,LOW);
  digitalWrite(REGCLK,LOW);
  shiftOut(SDA,SCL,MSBFIRST,address>>8);
  shiftOut(SDA,SCL,MSBFIRST,address);
  digitalWrite(REGCLK,HIGH);
}


/* EEPROM DATA BUS */


void dataReadBus(){
  for(int i=0;i<8;i++)
    pinMode(i+2,INPUT_PULLUP);
}

void dataWriteBus(){
  for(int i=0;i<8;i++)
    pinMode(i+2,OUTPUT);
}

void pushData(byte data){
  for(int i=0;i<8;i++){
    digitalWrite( (i+2) ,bitRead(data,i) );
  } 
}
