#ifndef ESP8266_H_
#define ESP8266_H_

#include "UART.h"
#include "Systick.h"

#define ESP8266_UART uart1
#define ESP8266_BAUDRATE 115200

typedef enum
{
	STATION_MODE,
	SOFTAP_MODE,
	SOFTAP_STATION_MODE
}Esp8266Mode;

typedef enum
{
	SINGLE_CONNECTION,
	MULTIPLE_CONNECTION
}CIPMUX_Value;

void SendATCommand(char *command);
void ReceiveResponse(char *response);
uint8 IsFound(char src[], char str[]);
uint8 Send_ATcmd();  //Test AT startup
uint8 Send_RSTcmd();  //Restart module
uint8 Send_CWMODEcmd(Esp8266Mode mode);  //set Wi-Fi mode(sta/AP/sta+AP)
uint8 Send_CWJAPcmd(const char *wifi_name, const char *wifi_password); //Connect to AP
uint8 Send_CWQAPcmd();  //Disconnect from AP
uint8 Send_CIPMUXcmd(CIPMUX_Value connection_type); // Enable multiple connections
uint8 Send_CIPSTARTcmd(const char *server_IP, const char *port);  //Establish TCP connection, UDP transmission or SSL connection
uint8 Send_CIPCLOSEcmd();  //Close TCP/UDP/SSL connection 
uint8 Send_CIPSENDcmd(char *text); //Send data
void ReadRecString(char *rec_string);


#endif /*ESP8266_H_*/
