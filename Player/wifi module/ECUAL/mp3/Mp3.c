#include "Mp3.h"

UARTNUM Mp3_UART; 

void MP3_Init(UARTNUM uart){
	UART_Init(uart,9600,Parity_OFF,Interrupt_OFF);
	SysTick_delay_ms(1500);
	Mp3_UART = uart;
}
void sendCommand(uint8 cmd , FEEDBACK feedback, uint16 Param){
	uint8 iter;
	uint8 Frame [8] ;

	Frame[0]=START_BYTE;
	Frame[1]=VER;
	Frame[2]=LENGTH;
	Frame[3]=cmd;
	Frame[4]=feedback;
	Frame[5]=Param>>8;
	Frame[6]=Param;
	Frame[7]=END_BYTE;
	
	
	for(iter = 0 ; iter < 8 ; iter++){
			UART_Write(Mp3_UART,Frame[iter]);
	}

}

void MP3_play(){
	sendCommand(PLAY,NO_FEEDBACK,00);
}

void MP3_pause(){
	sendCommand(PAUSE,NO_FEEDBACK,00);
}

void MP3_reset(){
	sendCommand(RESET,NO_FEEDBACK,00);
}

void MP3_increase_vol(){
	sendCommand(INCREASE_VOL,NO_FEEDBACK,00);
}

void MP3_decrease_vol(){
	sendCommand(DECREASE_VOL,NO_FEEDBACK,00);
}

void MP3_play_next(){	
	sendCommand(PLAY_NEXT,NO_FEEDBACK,00);
}

void MP3_play_previous(){	
	sendCommand(PLAY_PREVIOUS,NO_FEEDBACK,00);
}

void 	MP3_specify_track(uint16 track_no){
		sendCommand(SPECIFY_TRACK,NO_FEEDBACK,track_no);
}
