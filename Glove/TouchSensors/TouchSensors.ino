#include<ESP8266WiFi.h>
#include<Ticker.h>
#define UpperTouch D1 //Upper touch sensor pin
#define LowerTouch D2 //Lower touch sensor pin
#define RedLED D3
#define GreenLED D4
#define BlueLED  D5

String Action;  //Command to be snet via WiFi
bool UpperIsPressed= false;
bool LowerIsPressed=false;
bool UnPressed=false;

Ticker UpperTick, LowerTick, CheckStillPress, CheckHold, CheckStillPress2, CheckHold2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(UpperTouch,INPUT);
  pinMode(LowerTouch,INPUT);
  attachInterrupt(digitalPinToInterrupt(UpperTouch),UpperISR,RISING);
  attachInterrupt(digitalPinToInterrupt(LowerTouch),LowerISR,RISING);
  SetColor(0,255,0); //Green LED
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

void UpperISR(){
  //Serial.println("Upper is Touched");
  UpperIsPressed=true;
  LowerIsPressed=false;
  UpperTick.once_ms(500,CheckUpper);
}

void LowerISR(){
  //Serial.println("Lower is Touched");
  LowerIsPressed=true;
  UpperIsPressed=false;
  LowerTick.once_ms(500,CheckLower);
}

void CheckUpper(){
  if(LowerIsPressed){
    Action="L"; // Lowering Volume
    Serial.println(Action);
  }
  else if(UpperIsPressed) {
    Action="P"; //Pause
    Serial.println(Action);
  }
  LowerIsPressed= false;
  UpperIsPressed= false;
}

void CheckLower(){
  if(UpperIsPressed){
    Action="R"; // Raising Volume
    Serial.println(Action);
  }
  else {
    CheckStillPress.attach_ms(100,StillPressISR);
    CheckHold.once(2,HoldISR);
  }
  LowerIsPressed= false;
  UpperIsPressed= false;
}

void StillPressISR(){
  if(!digitalRead(LowerTouch)){
    UnPressed=true;
  }
}

void StillPressISR2(){
  if(!digitalRead(LowerTouch)){
    UnPressed=true;
  }
}

void HoldISR(){
  if(!UnPressed){
    Serial.println("I am Locked");
    detachInterrupt(UpperTouch);
    detachInterrupt(LowerTouch);
    CheckStillPress.detach();
    CheckHold.detach();
    SetColor(0,0,255); //Red LED
    UnPressed=false;
    Lock();
    }
    UnPressed=false;
    CheckStillPress.detach();
}

void Lock(){
    attachInterrupt(digitalPinToInterrupt(LowerTouch),LockISR,RISING);
}

void LockISR(){
  if (digitalRead(LowerTouch)) {
    CheckStillPress2.attach_ms(100, StillPressISR2);
    CheckHold2.once(2, UnHoldISR);
  }
}

void UnHoldISR(){
  if(!UnPressed){
    Serial.println("I am UnLocked");
    attachInterrupt(digitalPinToInterrupt(UpperTouch),UpperISR,RISING);
    attachInterrupt(digitalPinToInterrupt(LowerTouch),LowerISR,RISING);
    SetColor(0,255,0); //Green LED
    CheckStillPress2.detach();
    UnPressed=false;
    }
    else{
      CheckStillPress2.detach();
      Lock();
    }
    UnPressed=false;
}

void SetColor(int RedValue, int GreenValue, int BlueValue){
  digitalWrite(RedLED, RedValue);
  digitalWrite(GreenLED, GreenValue);
  digitalWrite(BlueLED, BlueValue);
}

