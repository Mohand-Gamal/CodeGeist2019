#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>
SoftwareSerial mySerial(D6,D7);
# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]
# define ACTIVATED LOW


IPAddress server(192,168,4,15);     // IP address of the AP
WiFiClient client;
int volume=20;
bool isPlaying=1;
char ssid[] = "HandyTech";           // SSID of your AP
char pass[] = "0123456789";         // password of your AP
String answer,request;

void setup () {
  Serial.begin (9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);           // connects to the WiFi AP
  delay(1000);
  playFirst();
  setVolume(volume);
}

void loop () {
  client.connect(server, 80);
  answer = client.readStringUntil('\r');
  Serial.println(answer);
  if(answer=="L"){
    volume=volume-5;
    setVolume(volume);
    }
    else if(answer=="R"){
      volume=volume+5;
      setVolume(volume);
    }
    else if(answer=="P"&&isPlaying==1){
      isPlaying=0;
      pause();
    }
    else if(answer=="P"&&isPlaying==0){
      isPlaying=1;
      play();
    }
    else if(answer=="E"){
      playNext();
    }
    else if(answer=="W"){
      playPrevious();
    }
}

void playFirst()
{
  execute_CMD(0x3F, 0, 0);
  delay(500);
  setVolume(20);
  delay(500);
  execute_CMD(0x11, 0, 1);
  delay(500);
}

void pause()
{
  execute_CMD(0x0E, 0, 0);
  delay(500);
}

void play()
{
  execute_CMD(0x0D, 0, 1);
  delay(500);
}

void playNext()
{
  execute_CMD(0x01, 0, 1);
  delay(500);
}

void playPrevious()
{
  execute_CMD(0x02, 0, 1);
  delay(500);
}

void setVolume(int volume)
{
  execute_CMD(0x06, 0, volume); // Set the volume (0x00~0x30)
  delay(2000);
}

void execute_CMD(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
  // Calculate the checksum (2 bytes)
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  // Build the command line
  byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
                            Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
                          };
  //Send the command line to the module
  for (byte k = 0; k < 10; k++)
  {
    mySerial.write( Command_line[k]);
  }
}
