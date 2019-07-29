#include <ESP8266WiFi.h>                  //Main Esp library
char ssid[] = "HandyTech";                //Username of the Access point
char pass[] = "Mo66778899";               //Password of the Access point
IPAddress server(192,168,1,11);           //IP address of the Access point
WiFiClient client;                        //Create the Client
String Action;                            //Variable to be recieved containing control actions

void setup() {
  Serial.begin(9600);                     //Initalize communication through serial monitor
  WiFi.mode(WIFI_STA);                    //Set the mode for the WiFi to Station
  WiFi.begin(ssid,pass);                  //Connect to the Access point WiFi
}

void loop() {
  client.connect(server, 80);             //Connect the Client to the Server
  Action = client.readStringUntil('\r');  //Read the action sent from the Access point
  if(Action != " "){                      //Check the action 
    client.print("D\r");                  //Respond with D when a valid action is recieved
    Serial.println(Action);
    }
}
