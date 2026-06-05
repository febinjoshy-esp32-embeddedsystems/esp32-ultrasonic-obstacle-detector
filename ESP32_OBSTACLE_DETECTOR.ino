#include<BluetoothSerial.h>
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
#define trig 5
#define echo 18
#define red 25
#define blue 26
#define buzzer 33
BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27,16,2);
bool in=false;
bool out=false;      
static unsigned long prev=0;
static bool state=false;  
unsigned long current;                                                                                                              
void setup() {
  Wire.begin(21,22);
  lcd.init();
  lcd.backlight();
  SerialBT.begin("ESP32 MODULE");
  pinMode(buzzer,OUTPUT);
  pinMode(red,OUTPUT);
  pinMode(blue,OUTPUT);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  lcd.setCursor(0,0);
  lcd.print("PAIR WITH ESP32");
  while(1){
    if(SerialBT.hasClient()){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CONNECTED");
      digitalWrite(buzzer,HIGH);
      digitalWrite(red,HIGH);
      digitalWrite(blue,HIGH);
      delay(1500);
      digitalWrite(red,LOW);
      digitalWrite(blue,LOW);
      digitalWrite(buzzer,LOW);
      lcd.clear();
      prev=0;
      current=0;
      state=false;
      break;    
    }
    else{
      lcd.setCursor(0,1);
      lcd.print("NOT CONNECTED");
      
       current=millis();

      if(current-prev>=200){
        prev=current;
        state=!state;

        if(state){
          digitalWrite(red,HIGH);
          digitalWrite(blue,LOW);
        }
        else{
          digitalWrite(red,LOW);
          digitalWrite(blue,HIGH);
        }
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);

  long duration=pulseIn(echo,HIGH,30000);
  float dcm = duration*0.0343/2;
  lcd.setCursor(0,0);
  if(duration==0||dcm>300){
    lcd.print("CLEAR");
    in=true;
    digitalWrite(red,LOW);
    digitalWrite(blue,HIGH);
    digitalWrite(buzzer,LOW);
  }else{
    lcd.print("OBSTACLE AHEAD");
    lcd.setCursor(0,1);
    lcd.print(dcm);
    lcd.print(" cm");
    if(SerialBT.hasClient()){
    SerialBT.print("OBSTACLE FOUND:");
    SerialBT.print(dcm);
    SerialBT.println(" cm");
    }
    out=true;
    digitalWrite(blue,LOW);
    digitalWrite(red,HIGH);

    current=millis();
    if(current-prev>=200){
      prev=current;
      state=!state;
      digitalWrite(buzzer,state?HIGH:LOW);
    }
  } 
  if(in&&out){
    lcd.clear();
    in=false;
    out=false;
    state=false;
  }
  delay(200);
}
