/*
 * encoder.c
 *
 * Created: 01.11.2019 13:42:50
 *  Author: sanderfu
 */ 

#include "encoder.h"
#include <stdio.h>
#define F_CPU 16000000

#include <util/delay.h>
#include <avr/io.h>

void encoder_init() {
	DDRH |= (1 <<PH5);
	DDRH |= (1 <<PH3);
	DDRH |= (1 <<PH6);

   
	//!Rst pin set high to use the encoder to know how much motor has rotated.
	PORTH &= ~(1 << PH5);
	_delay_us(20);

	PORTH |= (1 << PH6);
		_delay_us(20);

	PORTH &= ~(1 << PH6);
		_delay_us(20);

	PORTH |= (1 << PH6);
		_delay_us(20);

	PORTH |= (1 << PH5);


}
void encoder_readValues() {
	
	//set !OE value low
	PORTH &= ~(1 << PH5);
	
	//set select low to get high byte, then wait 20 microseconds
	PORTH &= ~(1 << PH3);
	_delay_us(20);
	
	//Read msb !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	uint8_t high_val = PINK & 0xff;

	
	//set select low to get high byte, then wait 20 microseconds
	PORTH |= (1 << PH3);
	_delay_us(20);
	//Read lsb !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	uint8_t low_val =  PINK & 0xff;

	
	
	//toggle reset pin
	
	//PORTH |= (1 << PH6);
	//PORTH &= ~(1 << PH6);
	//PORTH |= (1 << PH6);

	//Set !OE high
	PORTH |= (1 << PH5);
	
	//Process received data.
	int16_t rec_data = (high_val << 8) +low_val;
	
	//convert from two-complement
	if (rec_data >= 0) {
		encoder_value = rec_data;
	}
	else {
		encoder_value = (-1* (~rec_data +1));
	}
	if (encoder_value>0) {
		encoder_value = 0;
		
	} else if (encoder_value <-8368) {
		encoder_value = -8368;
	}
	encoder_convertValues();
	//printf("Encoder_values: %d\n\r", encoder_value);
}

void encoder_convertValues() {
	float a = -0.02309;
	float b = -100;
	converted_encoderValue = a*encoder_value + b;
	//printf("Converted encoder_values: %d\n\r", converted_encoderValue);
}