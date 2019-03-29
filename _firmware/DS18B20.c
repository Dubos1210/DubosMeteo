/*
 * DS18B20.c
 *
 * Created: 28.12.2018 14:31:56
 *  Author: Vladimir
 */ 

#include "settings.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "DS18B20.h"

uint8_t DS18B20_rst(void) {
	uint8_t ds18b20_ok = 0x00;
	PORTD &=~ (1<<6);             //Низкий уровень на шине, если выход
	DQ_out();
	_delay_us(500);
	DQ_in();
	_delay_us(60);
	if(!(DQ_pin())) {
		ds18b20_ok = 0xFF;
	}
	_delay_us(500);
	return ds18b20_ok;
}

void DS18B20_write(uint8_t ds18b20_data) {
	cli();
	for(uint8_t i = 0; i < 8; i++) {
		if(ds18b20_data & (1<<i)) {
			DQ_out();
			_delay_us(10);
			DQ_in();
			_delay_us(55);
		}
		else {
			DQ_out();
			_delay_us(60);
			DQ_in();
			_delay_us(5);
		}
	}
	sei();
}

uint8_t DS18B20_read(void) {
	cli();
	uint8_t ds18b20_rdata = 0;
	for(uint8_t i = 0; i < 8; i++) {
		DQ_out();
		_delay_us(5);
		DQ_in();
		_delay_us(15);
		if(DQ_pin()) {
			ds18b20_rdata |= (1<<i);
		}
		_delay_us(50);
	}
	sei();
	return ds18b20_rdata;
}

void DS18B20_start_meas(void) {
	if (DS18B20_rst() > 0) {
		DS18B20_write(SKIP_ROM);
		DS18B20_write(CONVERT);
		//_delay_ms(200);
	}
}

int16_t DS18B20_temperature(void) {
	int32_t ds18b20temperature = 0;
	if (DS18B20_rst() > 0) {
		DS18B20_write(SKIP_ROM);
		DS18B20_write(READ_SCRATCHPAD);
		ds18b20temperature = DS18B20_read();
		ds18b20temperature += DS18B20_read() << 8;
		ds18b20temperature *= 100;
		ds18b20temperature /= 16;
	}
	return ds18b20temperature;
}