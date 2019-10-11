/*
 * CAN.c
 *
 * Created: 25.09.2019 13:02:48
 *  Author: sanderfu
 */ 



#define F_CPU 4915200

#include <avr/io.h>
#include "MCP2515.h"
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "CAN_controller.h"
#include "SPI.h"
uint8_t CAN_controller_read(uint8_t addr) {
	SPI_setChipSelect(PB4, 0); 
	SPI_masterWrite(MCP_READ);
	SPI_masterWrite(addr);
	uint8_t data = SPI_masterRead();
	SPI_setChipSelect(PB4, 1);
	
	return data;
}

void CAN_controller_write(uint8_t addr, uint8_t data) {
	SPI_setChipSelect(PB4, 0);
	
	SPI_masterWrite(MCP_WRITE);
	SPI_masterWrite(addr);
	SPI_masterWrite(data);
	
	SPI_setChipSelect(PB4, 1); 
	
}
void CAN_controller_bitModify(uint8_t mask, uint8_t addr, uint8_t data) {
		SPI_setChipSelect(PB4, 0);
		SPI_masterWrite(MCP_BITMOD);
		SPI_masterWrite(addr);
		SPI_masterWrite(mask);
		SPI_masterWrite(data);
		
		SPI_setChipSelect(PB4, 1);

}
CAN_controller_reset() {
	SPI_setChipSelect(PB4, 0);
	printf("Before spi write");
	SPI_masterWrite(MCP_RESET);
	SPI_setChipSelect(PB4, 1);
}
void CAN_controller_init() {
	
	
	SPI_masterInit();
	/*
	//reset with spi command
	CAN_controller_reset();
	
	
	//printf("After spi write\n");
	_delay_ms(20);
	
	//Check CANSTAT register
	
	uint8_t status = CAN_controller_read(MCP_CANSTAT);
	_delay_ms(20);
	
	//uint8_t data = 0x44;
	uint8_t mode_bits = (status & MODE_MASK);
	
	if (mode_bits != MODE_CONFIG) {
		printf("Not in config mode, \t %i\n\r", mode_bits);
	}
	
	
	 //set in loopback mode p.60 MCP2515
	CAN_controller_bitModify(0b11101110, MCP_CANCTRL, MODE_LOOPBACK | (0b1100));
	
	*/
	CAN_controller_setMode(MODE_LOOPBACK);
	CAN_controller_bitModify(0b01100000, MCP_RXB0CTRL, 0b01100000); //receive any type of message, no filter p. 27
	CAN_controller_bitModify(0b11111111, MCP_CANINTE, 0b1);
	
	//set interrupt on atm162
	 GICR |= (1<< INT0); //turn on interrupt 0
	MCUCR |= (1 << ISC01); //Turn on falling edge
	MCUCR &= ~(1 << ISC00);
//set PD2 as input
	DDRD  &= (1 << PD2); //set as input.
	
	
	
	_delay_ms(200);
	printf("after write to canctrl\n\r");
	
	//Check CANSTAT register
	uint8_t status = CAN_controller_read(MCP_CANSTAT);
	printf("Data: %i\n\r", status);

}

void CAN_controller_RTS(uint8_t buffer) {
	SPI_setChipSelect(PB4, 0);
	switch (buffer) {
		case 0:
			SPI_masterWrite(MCP_RTS_TX0);
			break;
		case 1:
			SPI_masterWrite(MCP_RTS_TX1);
			break;
		case 2:
			SPI_masterWrite(MCP_RTS_TX2);
			break;
		default: 
			break;
			
	}
	SPI_setChipSelect(PB4, 1);
}

void CAN_controller_setMode(uint8_t mode) {
	
	CAN_controller_reset();
	uint8_t status = CAN_controller_read(MCP_CANSTAT);
	uint8_t mode_bits = (status & MODE_MASK);
	
	if (mode_bits != MODE_CONFIG) {
		printf("Not in config mode, \t %i\n\r", mode_bits);
		return;
	}
	
	
	 //set in loopback mode p.60 MCP2515
	CAN_controller_bitModify(0b11101110, MCP_CANCTRL, mode | (0b1100));
	_delay_ms(200);
	status = CAN_controller_read(MCP_CANSTAT);
	mode_bits = (status & MODE_MASK);
	if (mode_bits != mode) {
			printf("Not in correct mode: Mode: %i\n\r", mode_bits);

	}

}
