#include "esp8266.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

char* itoa(int value, char* result, int base);

char* itoa(int value, char* result, int base) 
{
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

void SendATCommand( char *command)
{
	UART_Print(ESP8266_UART, command);
	Putty_Print(command);
	UART_Write(ESP8266_UART, '\r');
	Putty_Print("\r");
	UART_Write(ESP8266_UART, '\n');
	Putty_Print("\n");
}

void ReceiveResponse(char *response)
{
	uint32 i=0;
	uint8 c;
	Putty_Print("hello");
	while(UART_Available(ESP8266_UART))
	{
		c = UART_Read(ESP8266_UART);
		if(c == '\0') continue;
		response[i]= c;
		i++;
	}
	Putty_Print(response);
	Putty_Print("\n");
	Putty_Print("\r");
}

uint8 IsFound(char src[], char str[])
{
   int i=0, j=0, first;
   while (src[i] != '\0')
   {
      while (src[i] != str[0] && src[i] != '\0')
			{
         i++;
			}

      if (src[i] == '\0')
			{
         return 0;
			}

      first = i;
      while (src[i] == str[j] && src[i] != '\0' && str[j] != '\0')
      {
         i++;
         j++;
      }

      if (str[j] == '\0')
         return 1;
      if (src[i] == '\0')
         return 0;

      i = first + 1;
      j = 0;
   }

   return 0;
}

uint8 Send_ATcmd()
{
	char res[512];
	SendATCommand("AT");
	ReceiveResponse(res);
	return IsFound(res, "OK");
}

uint8 Send_RSTcmd()
{
	char res[512];
	SendATCommand("AT+RST");
	ReceiveResponse(res);
	return IsFound(res, "OK");
}

uint8 Send_CWMODEcmd(Esp8266Mode mode)
{
	char res[512];
	switch(mode)
	{
		case STATION_MODE:
			SendATCommand("AT+CWMODE=1");
			break;
		case SOFTAP_MODE:
			SendATCommand("AT+CWMODE=2");
			break;
		case SOFTAP_STATION_MODE:
			SendATCommand("AT+CWMODE=3");
			break;
	}
	ReceiveResponse(res);
	return IsFound(res, "OK");
}

uint8 Send_CWJAPcmd(const char *wifi_name, const char *wifi_password)
{
	char res[512];
	char cmd[17]="AT+CWJAP=\"";
	strcat(cmd, wifi_name);
	strcat(cmd, "\",\"");
	strcat(cmd, wifi_password);
	strcat(cmd, "\"");
	SendATCommand(cmd);
	ReceiveResponse(res);
	return IsFound(res, "OK");
}

uint8 Send_CWQAPcmd()
{
	char res[512];
	SendATCommand("AT+CWQAP");
	ReceiveResponse(res);
	return IsFound(res, "OK");
	
}

uint8 Send_CIPMUXcmd(CIPMUX_Value connection_type)
{
	char res[512];
	switch(connection_type)
	{
		case SINGLE_CONNECTION:
			SendATCommand("AT+CIPMUX=0");
			break;
		case MULTIPLE_CONNECTION:
			SendATCommand("AT+CIPMUX=1");
			break;
	}
	ReceiveResponse(res);
	return IsFound(res, "OK");
}

uint8 Send_CIPSTARTcmd(const char *server_IP, const char *port)
{
	char res[512];
	char cmd[33]="AT+CIPSTART=\"TCP\",\"";
	strcat(cmd, server_IP);
	strcat(cmd, "\",");
	strcat(cmd, port);
	SendATCommand(cmd);
	ReceiveResponse(res);
	return IsFound(res, "CONNECT");
}

uint8 Send_CIPCLOSEcmd()
{
	char res[512];
	SendATCommand("AT+CIPCLOSE");
	ReceiveResponse(res);
	return IsFound(res, "OK");
	
}

uint8 Send_CIPSENDcmd(char *text)
{
	char str[10];
	char res[512];
	char cmd[12] = "AT+CIPSEND=";
	int len = strlen(text)+2;
	itoa(len,str,10);
	strcat(cmd,str);
	SendATCommand(cmd);
	ReceiveResponse(res);
	SendATCommand(text);
	ReceiveResponse(res);
	return IsFound(res, "SEND OK");
}

void ReadRecString(char *rec_string)
{
	UART_Read(ESP8266_UART);
	
	
}