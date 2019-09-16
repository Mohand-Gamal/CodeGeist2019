#ifndef MP3_H_
#define MP3_H_

#include "UART.h"

#define START_BYTE 0x7E
#define VER 0xFF 
#define LENGTH 0x06
#define END_BYTE 0xEF

typedef enum {
  NO_FEEDBACK,
	NEED_FEEDBACK
} FEEDBACK;


#define PLAY_NEXT 0x01
#define PLAY_PREVIOUS 0x02
#define SPECIFY_TRACK 0x03
#define INCREASE_VOL 0x04
#define DECREASE_VOL 0x05
#define SPECIFY_VOL 0x06
#define SPECIFY_SINGLE_REPEAT 0x08
#define SPECIFY_DEVICE 0x09
#define RESET 0x0C
#define PLAY 0x0D
#define PAUSE 0x0E
#define STOP 0x16
#define SET_RANDOM_PLAYBACK 0x18
#define REPEAT_CURRENT_TRACK 0x19


void MP3_Init(UARTNUM uart);
void sendCommand(uint8 cmd , FEEDBACK feedback, uint16 Param);
void MP3_play();
void MP3_pause();
void MP3_reset();
void MP3_increase_vol();
void MP3_decrease_vol();
void MP3_play_next();
void MP3_play_previous();
void 	MP3_specify_track(uint16 track_no);
#endif /*MP3_H_*/ 