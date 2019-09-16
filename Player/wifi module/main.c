#include "systick.h"
#include "esp8266.h"
#include "RGB_LED.h"
#include "PLL.h"

char response[512];

int main(void)
{	
	uint8 state=0;
	PLL_Init();
	SysTick_Init();
	Putty_Init();
	UART_Init(ESP8266_UART,ESP8266_BAUDRATE,Parity_OFF,Interrupt_OFF);
	RGB_LED_Init();
	RGB_LED_Set_Color(green);
	
	while(!state)
	{
		state=Send_ATcmd();
		SysTick_delay_ms(10);
	}
	
	state=0;
	while(!state)
	{
		state=Send_RSTcmd();
		SysTick_delay_ms(10);
	}
	
	
	state=0;
	while(!state)
	{
		state=Send_CWMODEcmd(STATION_MODE);
		SysTick_delay_ms(10);
	}
	
	
	state=0;
	while(!state)
	{
		state=Send_CWJAPcmd("AMZ","amz26017532959820");
		SysTick_delay_ms(30);
	}
	RGB_LED_Set_Color(white);
	
	
	while(1)
	{
		/*
		char cmd[100];
		uint8 i=0;
		uint8 c=Putty_Read();
		while(c != ' ')
		{
			Putty_Write(c);
			cmd[i]=c;
			i++;
			c=Putty_Read();
		}
		cmd[i]='\0';
		Putty_Print("\n\r");
		
		UART_Print(ESP8266_UART, cmd);
		UART_Write(ESP8266_UART, '\r');
		UART_Write(ESP8266_UART, '\n');
		
	
		while(UART_Available(ESP8266_UART))
		{
			char r[512];
			uint32 j=0;
			uint8 ch;
			ch = UART_Read(ESP8266_UART);
			if(ch == '\0') continue;
			r[j]= ch;
			response[j]=r[j];
			
			j++;
		}
		Putty_Print(response);
		Putty_Print("\n");
		Putty_Print("\r");
		*/
		
		state=0;
		while(!state)
		{
			state=Send_CIPMUXcmd(SINGLE_CONNECTION);
			SysTick_delay_ms(10);
		}
			
		state=0;
		while(!state)
		{
			state=Send_CIPSTARTcmd("api.thingspeak.com","80");
			SysTick_delay_ms(20);
		}
		RGB_LED_Set_Color(red);
		
		state=0;
		while(!state)
		{
			state=Send_CIPSENDcmd("GET /update?api_key=QHFM8WIDQJM27SYU&field1=50\r\n");
			SysTick_delay_ms(2);
		}
		RGB_LED_Set_Color(pink);
		
		SysTick_delay_ms(1500);
		
		/*
		state=0;
		while(!state)
		{
			state=Send_CIPCLOSEcmd();
			SysTick_delay_ms(100);
		}
		Putty_Print("stuck");
		SysTick_delay_ms(10000);
		*/
	}
}
