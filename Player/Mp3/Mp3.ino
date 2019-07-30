#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>

SoftwareSerial mySerial(D6,D7);

# define START_BYTE 0x7E
# define VERSION_BYTE 0xFF
# define COMMAND_LENGTH 0x06
# define END_BYTE 0xEF
# define ACK 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]
# define ACTIVATED LOW

//Commands Definations 
#define PLAY_NEXT 0x01
#define PLAY_PREVIOUS 0x02 
#define INCREASE_VOL 0x04
#define DECREASE_VOL 0x05
#define PLAY 0x0D
#define PAUSE 0x0E
#define SPECIFY_TRACK 0x3F
#define SPECIFY_VOL 0x06
#define REPEAT_PLAY 0x11



IPAddress server(192,168,4,15);     // IP address of the AP
WiFiClient client;
int volume = 20;
bool isPlaying = true;
char ssid[] = "HandyTech";           // SSID of your AP
char pass[] = "0123456789";         // password of your AP
String answer,request;

void setup () {
  Serial.begin (9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);           // connects to the WiFi AP
  delay(1000);
  Mp3_playFirst();
  Mp3_setVolume(volume);
}

void loop () {
  
  client.connect(server, 80);
  answer = client.readStringUntil('\r');
  Serial.println(answer);

  Mp3_playNext();

  delay(5000);
  Mp3_playNext();

  delay(5000);
  Mp3_playNext();

  delay(5000);
  if(answer == "L"){
      Mp3_volumeDown();
  }
  else if(answer == "R"){
      Mp3_volumeUp();
  }
  else if(answer == "P" && isPlaying){
      isPlaying = false;
      Mp3_pause();
  }
  else if(answer == "P" && !isPlaying){
      isPlaying = true;
      Mp3_play();
  }
  else if(answer == "E"){
      Mp3_playNext();
  }
  else if(answer == "W"){
      Mp3_playPrevious();
  }
}

void Mp3_playFirst()
{
  execute_CMD(SPECIFY_TRACK, 0, 0);
  delay(500);
  Mp3_setVolume(20);
  delay(500);
  execute_CMD(0x11, 0, 1);
  delay(500);
}

void Mp3_pause()
{
  execute_CMD(PAUSE, 0, 0);
  delay(500);
}

void Mp3_play()
{
  execute_CMD(PLAY, 0, 1);
  delay(500);
}

void Mp3_playNext()
{
  execute_CMD(PLAY_NEXT, 0, 1);
  delay(500);
}

void Mp3_playPrevious()
{
  execute_CMD(PLAY_PREVIOUS, 0, 1);
  delay(500);
}
void Mp3_volumeUp(){
  volume = volume+5;
  Mp3_setVolume(volume);  
}
void Mp3_volumeDown(){
  volume = volume-5;
  Mp3_setVolume(volume);  
}
void Mp3_setVolume(int volume)
{
  execute_CMD(SPECIFY_VOL, 0, volume); // Set the volume (0x00~0x30)
  delay(2000);
}

void execute_CMD(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
  // Calculate the checksum (2 bytes)
  word checksum = -(VERSION_BYTE + COMMAND_LENGTH + CMD + ACK + Par1 + Par2);
  // Build the command line
  byte Command_line[10] = { START_BYTE, VERSION_BYTE, COMMAND_LENGTH, CMD, ACK,
                            Par1, Par2, highByte(checksum), lowByte(checksum), END_BYTE
                          };
  //Send the command line to the module
  for (byte k = 0; k < 10; k++)
  {
    mySerial.write( Command_line[k]);
  }
}
