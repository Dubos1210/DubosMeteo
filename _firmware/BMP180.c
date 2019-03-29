/*
 * BMP180.c
 *
 * Created: 29.12.2018 22:37:29
 *  Author: Vladimir
 */ 

#include "settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <math.h>
#include "I2C.h"
#include "BMP180.h"

int16_t BMP180_AC1;
int16_t BMP180_AC2;
int16_t BMP180_AC3;
uint16_t BMP180_AC4;
uint16_t BMP180_AC5;
uint16_t BMP180_AC6;
int16_t BMP180_B1;
int16_t BMP180_B2;
int16_t BMP180_MB;
int16_t BMP180_MC;
int16_t BMP180_MD;

double c5, c6, mc, md, x0, x1, x2, y0, y1, y2, p0, p1, p2;

uint8_t BMP180_buffer[3];

double BMP180_UP, BMP180_UT;

uint8_t UART_buff[16];	//DEBUG

uint8_t BMP180_init(void) {
	//Чтение ID чипа (проверка связи)
	I2C_read(BMP180_ADDR, BMP180_DEVICE_ID, &BMP180_buffer, 1);
	if(BMP180_buffer[0] != BMP180_ID) {
		return 0;
	}
			
	//Чтение калибровочных констант
	
	I2C_read(BMP180_ADDR, 0xAA, &BMP180_buffer, 2);
	BMP180_AC1 = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xAC, &BMP180_buffer, 2);
	BMP180_AC2 = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xAE, &BMP180_buffer, 2);
	BMP180_AC3 = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xB0, &BMP180_buffer, 2);
	BMP180_AC4 = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xB2, &BMP180_buffer, 2);
	BMP180_AC5 = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xB4, &BMP180_buffer, 2);
	BMP180_AC6 = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xB6, &BMP180_buffer, 2);
	BMP180_B1 = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xB8, &BMP180_buffer, 2);
	BMP180_B2 = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xBA, &BMP180_buffer, 2);
	BMP180_MB = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xBC, &BMP180_buffer, 2);
	BMP180_MC = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	I2C_read(BMP180_ADDR, 0xBE, &BMP180_buffer, 2);
	BMP180_MD = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
	
	//BMP180_AC1 = 408;
	//BMP180_AC2 = -72;
	//BMP180_AC3 = -14383;
	//BMP180_AC4 = 32741;
	//BMP180_AC5 = 32757;
	//BMP180_AC6 = 23153;
	//BMP180_B1 = 6190;
	//BMP180_B2 = 4;
	//BMP180_MB = -32768;
	//BMP180_MC = -8711;
	//BMP180_MD = 2868;
	
	// Compute floating-point polynominals:
		
	double c3, c4, b1;

	c3 = 160.0 * pow(2,-15) * BMP180_AC3;
	c4 = pow(10,-3) * pow(2,-15) * BMP180_AC4;
	b1 = pow(160,2) * pow(2,-30) * BMP180_B1;
	c5 = (pow(2,-15) / 160) * BMP180_AC5;
	c6 = BMP180_AC6;
	mc = (pow(2,11) / pow(160,2)) * BMP180_MC;
	md = BMP180_MD / 160.0;
	x0 = BMP180_AC1;
	x1 = 160.0 * pow(2,-13) * BMP180_AC2;
	x2 = pow(160,2) * pow(2,-25) * BMP180_B2;
	y0 = c4 * pow(2,15);
	y1 = c4 * c3;
	y2 = c4 * b1;
	p0 = (3791.0 - 8.0) / 1600.0;
	p1 = 1.0 - 7357.0 * pow(2,-20);
	p2 = 3038.0 * 100.0 * pow(2,-36);
		
	return 1;
}

static void inline BMP180_get_ut() {	
	BMP180_buffer[0] = BMP180_COMMAND_TEMPERATURE;
	I2C_write(BMP180_ADDR, BMP180_REG_CONTROL, &BMP180_buffer, 1);
	_delay_ms(5);
	I2C_read(BMP180_ADDR, BMP180_REG_RESULT, &BMP180_buffer, 2);
	BMP180_UT = (BMP180_buffer[0] << 8) + BMP180_buffer[1];
}

static void inline BMP180_get_up() {
	BMP180_buffer[0] = BMP180_COMMAND_PRESSURE0;
	I2C_write(BMP180_ADDR, BMP180_REG_CONTROL, &BMP180_buffer, 1);
	_delay_ms(30);
	I2C_read(BMP180_ADDR, BMP180_REG_RESULT, &BMP180_buffer, 3);
	//BMP180_UP = ((BMP180_buffer[0] << 16) + (BMP180_buffer[1] << 8) + BMP180_buffer[1]) >> (8-BMP180_OSS_S);
	BMP180_UP = (BMP180_buffer[0] * 256.0) + BMP180_buffer[1] + (BMP180_buffer[2]/256.0);
}

void BMP180_calculation (int32_t* temperature, int32_t* pressure) {
	double a, s, x, y, z;
	int32_t t, p;
	
	BMP180_get_ut();
	a = c5 * (BMP180_UT - c6);
	t = (a + (mc / (a + md))) * 10;
	*temperature = t;

	BMP180_get_up();
	s = (t - 250) / 10;
	x = (x2 * pow(s,2)) + (x1 * s) + x0;
	y = (y2 * pow(s,2)) + (y1 * s) + y0;
	z = (BMP180_UP - x) / y;
	p = ((p2 * pow(z,2)) + (p1 * z) + p0) * 100;
	*pressure = p;
}