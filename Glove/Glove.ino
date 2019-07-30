#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Ticker.h>
#define TouchUpper D2
#define TouchLower D1
#define RedLed D3
#define GreenLed D4
#define BlueLed D5

void SetColor(int RedValue, int GreenValue, int BlueValue);
void UpperISR();
void LowerISR();
void TimerUpperISR();
void TimerLowerISR();
void Command();
void CheckPress();
void CheckHold();
void CheckUnHold();
void Hold();
void HoldISR();
void mpu_ISR();
void mpuReset();
void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data);
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress);
void MPU6050_Init();
void SetColor(int RedValue, int GreenValue, int BlueValue);

WiFiServer server(80);
IPAddress IP(192,168,4,15);
IPAddress mask = (255, 255, 255, 0);

// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication
const uint8_t scl = D6;
const uint8_t sda = D7;

// sensitivity scale factor respective to full scale setting provided in datasheet
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

String ActionToBeDone;
double mpu_value;
bool LowerIsTouched = 0;
bool UpperIsTouched = 0;
bool PressToHold = 1;
bool PressToUnHold = 1;
bool BreakFree = 0;
bool mpuchange = 0;
int sentP=0, sentR=0, sentL=0;

Ticker TimerLower, TimerUpper, TimerShort1, TimerShort2, TimerHold1, TimerHold2, mpu_tick, pushcommandP, pushcommandR, pushcommandL;
int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("HandyTech", "0123456789");
  WiFi.softAPConfig(IP, IP, mask);
  server.begin();
  Serial.begin(9600);
  
  pinMode(TouchUpper, INPUT);
  pinMode(TouchLower, INPUT);
  pinMode(RedLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(BlueLed, OUTPUT);

  SetColor(0,255,0); //Green
    
  attachInterrupt(digitalPinToInterrupt(TouchUpper), UpperISR , RISING);
  attachInterrupt(digitalPinToInterrupt(TouchLower), LowerISR , RISING);
  Wire.begin(sda, scl);
  MPU6050_Init();
  mpu_tick.attach_ms(100, mpu_ISR);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mpuchange) {
    mpuReset();
  }
  WiFiClient client = server.available();
  if (!client) {return;}
  //client.flush();
  client.println(ActionToBeDone+"\r");
  ActionToBeDone="";
}

void TimerLowerISR() {
  if (LowerIsTouched) {
    ActionToBeDone = "L";
    pushcommandL.attach_ms(10,CommandL);
    Serial.println(ActionToBeDone);
  }
  else if (UpperIsTouched) {
    ActionToBeDone = "P";
    pushcommandP.attach_ms(10,CommandP);
    Serial.println(ActionToBeDone);
  }
  LowerIsTouched = 0;
}

void CommandP(){
  ActionToBeDone = "P";
  sentP++;
  if(sentP==10){
    sentP=0;
    pushcommandP.detach();}
}

void CommandL(){
  ActionToBeDone = "L";
  sentL++;
  if(sentL==10){
    sentL=0;
    pushcommandL.detach();}
}

void CommandR(){
  ActionToBeDone = "R";
  sentR++;
  if(sentR==10){
    sentR=0;
    pushcommandR.detach();}
}

void TimerUpperISR() {
  if (UpperIsTouched) {
    ActionToBeDone = "R";
    pushcommandR.attach_ms(10,CommandR);
    Serial.println(ActionToBeDone);
  }
  else if (LowerIsTouched) {
    TimerShort1.attach_ms(100, CheckPress);
    TimerHold1.once(2, CheckHold);
  }
  UpperIsTouched = 0;
}

void UpperISR() {
  //Serial.println("Upper Touched");
  UpperIsTouched = 1;
  LowerIsTouched = 0;
  TimerLower.once_ms(500, TimerLowerISR); //500000 us
}

void LowerISR() {
  //Serial.println("Lower Touched");
  LowerIsTouched = 1;
  UpperIsTouched = 0;
  TimerUpper.once_ms(500, TimerUpperISR); //500000 us

}

void CheckPress() {
  if (!digitalRead(TouchLower)) {
    PressToHold = 0;
    PressToUnHold = 0;
    //Serial.println("Not Press");
  }
  else {
    //Serial.println("Still Press");
  }
}

void CheckHold() {
  if (PressToHold) {
    //Serial.println("Press and Hold for 5 Seconds, LOCKED");
    TimerShort1.detach();
    detachInterrupt(TouchUpper);
    detachInterrupt(TouchLower);
    mpuchange=0;
    mpu_tick.detach();
    delay(100);
    Hold();
  }
  PressToHold = 1;
  TimerShort1.detach();
}

void CheckUnHold() {
  if (PressToUnHold) {
    TimerShort2.detach();
    Serial.println("I Broke Free From Lock :D");
    //ActionToBeDone = "UnLocked";
    SetColor(0,255,0); //Green
    digitalWrite(D8, LOW);
    attachInterrupt(digitalPinToInterrupt(TouchUpper), UpperISR , RISING);
    attachInterrupt(digitalPinToInterrupt(TouchLower), LowerISR , RISING);
    mpu_tick.attach_ms(100, mpu_ISR);
  }
  else {
    TimerShort2.detach();
    Hold();
  }
  PressToUnHold = 1;
}

void Hold() {
  Serial.println("I am Stuck In Hold :(");
  ActionToBeDone = "H";
  SetColor(255,0,0); //Red
  digitalWrite(D8, HIGH);
  attachInterrupt(digitalPinToInterrupt(TouchLower), HoldISR , RISING);
}

void HoldISR() {
  if (digitalRead(TouchLower)) {
    TimerShort2.attach_ms(100, CheckPress);
    TimerHold2.once(2, CheckUnHold);
  }
}

void mpu_ISR() {
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  mpu_value = (double)AccelY / AccelScaleFactor;
  //Serial.println(mpu_value);
  if (mpu_value>=0.9) {
    mpu_tick.detach();
    ActionToBeDone = "W";
    Serial.println(ActionToBeDone);
    mpuchange=1;
  }
  else if(mpu_value <= -0.9) {
    mpu_tick.detach();
    ActionToBeDone = "E";
    Serial.println(ActionToBeDone);
    mpuchange=1;
  }
}

void mpuReset(){
  mpuchange=0;
  mpu_tick.attach_ms(100, mpu_ISR);
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read() << 8) | Wire.read());
  AccelY = (((int16_t)Wire.read() << 8) | Wire.read());
  AccelZ = (((int16_t)Wire.read() << 8) | Wire.read());
  Temperature = (((int16_t)Wire.read() << 8) | Wire.read());
  GyroX = (((int16_t)Wire.read() << 8) | Wire.read());
  GyroY = (((int16_t)Wire.read() << 8) | Wire.read());
  GyroZ = (((int16_t)Wire.read() << 8) | Wire.read());
}

//configure MPU6050
void MPU6050_Init() {
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}

void SetColor(int RedValue, int GreenValue, int BlueValue){
  digitalWrite(RedLed, RedValue);
  digitalWrite(GreenLed, GreenValue);
  digitalWrite(BlueLed, BlueValue);
}

