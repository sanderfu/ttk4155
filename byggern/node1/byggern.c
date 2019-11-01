/*
 * byggern.c
 *
 * Created: 30.08.2019 15:12:42
 *  Author: torsteoe
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#define F_CPU 4915200
#include <avr/interrupt.h>
#include "menu.h"
#include <util/delay.h>
#include "setup.h"

#include <stdlib.h>
#include "uart.h"
#include "xmem.h"
#include "adc.h"
#include "joystick.h"
#include "slider.h"
#include "touchButton.h"
#include "oled.h"
#include "avr/interrupt.h"
#include "menu.h"
#include "timer.h"
#include "test.h"
#include "SPI.h"
#include "CAN_controller.h"
#include "MCP2515.h"
#include "pwm.h"
#include "music.h"
#include "CAN.h"
volatile CAN_message_t received_message;
volatile uint8_t flag = 0;
ISR (INT0_vect) {
	//cli();
	flag= 1;
	
	
	//sei();
	
}


int main(void)
//p.23 for can read instructions
{

	setupInit();
	
	//test_SRAM();
	volatile CAN_message_t message;
	message.ID = 0b10101010111;
	message.data_length = 3;
	message.data[0] = 13;
	message.data[1] = 22;
	message.data[2] = 33;
	
	printf("Starting program\n\r");
	printf("---------------------------------------\n\r");
	_delay_ms(500);
	//CAN_transmit_message(&message);
	
	//CAN_controller_setMode(MODE_NORMAL);
	//music_playRick();

	//test_SRAM();
	//pwm_testPlayNote();
	//pwm_init();
	//music_playLisaGikk();                 
	while (1) {
		//
		//Put microcontroller to sleep until next interrupt. 

		sleep_now();
		
		if (flag) {
			printf("Message received");
			flag=0;
			CAN_receiveMessage(&received_message);
			//printf("This is the data: %i", received_message.data);
			uint8_t mask = 0b11; 
			CAN_controller_bitModify(mask, CANINTF, 0b00);
			
		}
		
		joystick_sendPositionOverCAN();
		slider_sendPositionOverCAN();
	}
}
