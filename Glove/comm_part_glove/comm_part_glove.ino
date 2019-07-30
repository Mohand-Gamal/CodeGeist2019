#include <ESP8266WiFi.h>                          //Main Esp library
WiFiServer server(80);                            //Server initialization inside the Node mcu
IPAddress IP(192,168,1,11);                       //IP address of the access point
IPAddress mask = (255, 255, 255, 0);              //Subnet mask for IP
String Action;                                    //Variable to be sent containing control actions
String Respond;                                   //Variable to store the respond from Station

void setup() {
  Serial.begin(9600);                             //Initalize communication through serial monitor
  WiFi.mode(WIFI_AP);                             //Set the mode for the WIFI to Access point
  WiFi.softAP("HandyTech", "Mo66778899");         //Set the name and password for the WIFI network
  WiFi.softAPConfig(IP, IP, mask);                //Configration for the Access point
  server.begin();                                 //Initialize the Server 
  Action = "F";                                 //For testing
}

void loop() {
  WiFiClient client = server.available();         //Create the Station connected to the Server
  if(!client){                                    //Check if the Station is connected
    return;
    }
  do{
    Serial.print("Action sent to the player :");  
    Serial.println(Action);                       
    client.println(Action + "\r");                //Send the action to the Station     
    Respond =client.readStringUntil('\r');        //Read respond from Station
    Serial.println(Respond);
    } while(Respond != "D");                      //Wait until the Station respond with D
    Action = " ";                                 //Reset the Action variable
}
