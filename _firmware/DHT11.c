/*
 * DHT11.c
 *
 * Created: 21.03.2019 15:24:13
 *  Author: Vladimir
 */

#include "settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "DHT11.h"

uint8_t DHT11_getData(int8_t* temperature, uint8_t* humidity) {
	//Setting pins
	DHT_DDR &=~ (1<<DHT_WIRE);
	DHT_PORT &=~ (1<<DHT_WIRE);
	
	//Checking data line
	if(!(DHT_PIN & (1<<DHT_WIRE))) {
		_delay_ms(10);
		if(!(DHT_PIN & (1<<DHT_WIRE))) return DHT11_ERROR;
	}
	
	//Start signal
	DHT_DDR |= (1<<DHT_WIRE);
	_delay_ms(20);	
	DHT_DDR &=~ (1<<DHT_WIRE);
	_delay_us(60);
	if(DHT_PIN & (1<<DHT_WIRE)) {
		return DHT11_ERROR;
	}
	while(!(DHT_PIN & (1<<DHT_WIRE)));
	while(DHT_PIN & (1<<DHT_WIRE));
	
	//Data bytes
	uint8_t DHT11_RH_integral = DHT11_readByte();
	DHT11_readByte();
	uint8_t DHT11_T_integral  = DHT11_readByte();
	DHT11_readByte();
	DHT11_readByte();
	
	*humidity = DHT11_RH_integral;
	*temperature = DHT11_T_integral;
		
	return DHT11_OK;
}

uint8_t DHT11_readByte(void) {
	uint8_t DHT11_counter = 0;
	uint8_t DHT11_data = 0x00;	
	DHT_DDR &=~ (1<<DHT_WIRE);
	for(int DHT11_i = 7; DHT11_i >= 0; DHT11_i--) {
		DHT11_counter = 0;
		while(!(DHT_PIN & (1<<DHT_WIRE)) && (DHT11_counter < 10)) {
			_delay_us(10);
			DHT11_counter++;
		}
		DHT11_counter = 0;
		while((DHT_PIN & (1<<DHT_WIRE)) && (DHT11_counter < 15)) {
			_delay_us(10);
			DHT11_counter++;
		}
		if(DHT11_counter > 5) {
			DHT11_data += (1<<DHT11_i);
		}
	}
	return DHT11_data;
}
