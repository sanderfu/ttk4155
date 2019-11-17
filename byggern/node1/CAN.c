/*
 * CAN.c
 *
 * Created: 04.10.2019 15:12:56
 *  Author: 
 */ 


#include "CAN.h"
#include "CAN_controller.h"
#include "MCP2515.h"
#include "slider.h"
#include "joystick.h"
#include "touchButton.h"
#include <stdio.h>
#define F_CPU 4915200
#include <util/delay.h>


void CAN_clearInterrupt()
{
	//CLearing interrupt
	uint8_t mask = 0b11;
	CAN_controller_bitModify(mask, CANINTF, 0b00);
}
void CAN_transmit_message(CAN_message_t *message) {
	//printf("Transmitting message");
	static int buffer_number = 0;
	
	buffer_number++;
	buffer_number = buffer_number%3;
	
	//Load id in registers TXBnSIDH and TXBnSIDL
	switch(buffer_number) {
			case 0:
				CAN_controller_bitModify(0b11100000, TXB0SIDL, (uint8_t) ((message->ID & 0b111) << 5));
				CAN_controller_write(TXB0SIDH, (uint8_t) ( message->ID >> 3) );
				
				//printf("Sending whole id: %i", message->ID );


				break;
			case 1:
				CAN_controller_bitModify(0b11100000, TXB1SIDL, (uint8_t) ((message->ID & 0b111) << 5));
				CAN_controller_write(TXB1SIDH, (uint8_t) ( message->ID >> 3) );
				
				break;
			case 2:
				CAN_controller_bitModify(0b11100000, TXB2SIDL, (uint8_t) ((message->ID & 0b111) << 5));
				CAN_controller_write(TXB2SIDH, (uint8_t) ( message->ID >> 3) );
				break;
			
			
	}
		
	//Load length in register TXBnDLC
	switch(buffer_number) {
			case 0:
				CAN_controller_bitModify(0b1111, TXB0DLC, message->data_length);
				break;
			case 1:
				CAN_controller_bitModify(0b1111, TXB1DLC, message->data_length);
				break;
			case 2:
				CAN_controller_bitModify(0b1111, TXB2DLC, message->data_length);
				break;
			
			
	}

	//iterate in for loop length = message.length and load TXBnDm
	for (uint8_t i = 0; i != message->data_length; i++) {
		switch(buffer_number) {
			case 0:
				//printf("transmitting data: %i\t", message->data[i]);
				CAN_controller_write(TXB0D0 + i, message->data[i]);
				break;
			case 1:
				CAN_controller_write(TXB1D0 + i, message->data[i]);
				break;
			case 2:
				CAN_controller_write(TXB2D0 + i, message->data[i]);
				break;
			
			
		}
		
	}
	

	
	//request_to_send

	CAN_controller_RTS(buffer_number);
	
	//ERROR HANDLING?	
}

void CAN_receiveMessage(volatile CAN_message_t * received_message) {
	
	
	uint8_t buffer = 0;
	switch (buffer) {
		case 0:
				received_message->ID = ((CAN_controller_read(MCP_RXB0SIDL) & 0b11100000) >> 5) + (CAN_controller_read(MCP_RXB0SIDH) << 3); 
				
				//printf("Whole id: %i\n\r", received_message->ID);
				received_message->data_length = (CAN_controller_read(MCP_RXB0DLC) & 0b00001111);
				//printf("data length read: %i\n\r",(CAN_controller_read(MCP_RXB0DLC) & 0b00001111));
				for (uint8_t i = 0; i != received_message->data_length; i++) {
					received_message->data[i] = CAN_controller_read(MCP_RXB0D0 + i);
					//printf("data: %i\t", received_message->data[i]);

				}
				break;
		case 1:
				received_message->ID = ((CAN_controller_read(MCP_RXB1SIDL) & 0b11100000) >> 5) + (CAN_controller_read(MCP_RXB1SIDH) << 3); 
				
				printf("Whole id: %i\n\r", received_message->ID);
				received_message->data_length = (CAN_controller_read(MCP_RXB1DLC) & 0b00001111);
				printf("data length read: %i\n\r",(CAN_controller_read(MCP_RXB1DLC) & 0b00001111));
				for (uint8_t i = 0; i != received_message->data_length; i++) {
					received_message->data[i] = CAN_controller_read(MCP_RXB1D0 + i);
					printf("data: %i\t", received_message->data[i]);

				}
				break;
	}
	
	
	
	
}

void CAN_sendInputData() {
	CAN_message_t message;
	
	message.ID = 0x01;
	message.data_length = 6;
	
	message.data[0] = slider_pos.left_pos;
	message.data[1] = slider_pos.right_pos;
	message.data[2] = buttons.left_button;
	message.data[3] = buttons.right_button;
	message.data[4] = joystick_pos.x_pos;
	message.data[5] = joystick_pos.y_pos;
	CAN_transmit_message(&message);
}

void CAN_init() {
	CAN_controller_init();
}
