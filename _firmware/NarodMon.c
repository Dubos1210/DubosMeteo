/*
 * NarodMon.c
 *
 * Created: 27.03.2019 12:43:55
 *  Author: Vladimir
 */ 

#include "settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "UART.h"
#include "I2C.h"
#include "BMP180.h"
#include "DS18B20.h"
#include "DHT11.h"

uint8_t DHT_temp = 20;
uint8_t DHT_hum = 50;
int32_t BMP_temp = 200;
int32_t BMP_press = 7600;
int16_t DS_temp = 20;
uint8_t UART_counter = 0;
uint8_t UART_data[32];
char buf[94];

uint16_t temp;

void static inline LED_in() {
	OCR1B = 0x3FF;
}
void static inline LED_fadein() {
	while(OCR1B < 0x3FF) {		
		OCR1B++;
		_delay_ms(2);
	}
}
void static inline LED_off() {
	OCR1B = 0;
}
void static inline LED_fadeout() {
	while(OCR1B > 0) {
		OCR1B--;
		_delay_ms(2);
	}
}

ISR(USART_RXC_vect) {
	LED_fadeout();
}

int main(void)
{
	_delay_ms(5000);
	
	DDRB |= (1<<2);	//LED
	TCCR1A = (0<<COM1A1)|(0<<COM1A0)|(1<<COM1B1)|(0<<COM1B0)|(0<<FOC1A)|(0<<FOC1B)|(1<<WGM11)|(1<<WGM10);
	TCCR1B = (0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(1<<WGM12)|(0<<CS12)|(1<<CS11)|(0<<CS10);
	OCR1B = 0x0000;
	LED_fadein();
	
	//ESP8266
	UART_init(76800);
	UBRRH = 0;
	UBRRL = 8;
	UCSRA |= (1<<U2X);
	UART_send_string("AT+RST\r\n");
	_delay_ms(5000);
	UART_send_string("AT+CWJAP=\"DubosSouth\",\"dubos2018\"\r\n");
	_delay_ms(5000);
	UART_send_string("AT+CIPMUX=0\r\n");
	_delay_ms(100);
	
	//strcpy(buf, "#cc:50:e3:2b:55:fc\n#DS18B20#000.00\n#DHT11_T#000\n#DHT11_H#00\n#BMP180_T#000.0\n#BMP180_P#000.0\n##");
	strcpy(buf, "#cc:50:e3:2b:55:fc\n#DS18B20#000.00\n#DHT11_T#000\n#DHT11_H#00\n##");
	
	BMP180_init();
	
	// Initialize Dallas DS18B20
	if(DS18B20_rst() > 0) {
		DS18B20_write(SKIP_ROM);
		DS18B20_write(WRITE_SCRATCHPAD);
		DS18B20_write(0xFF);
		DS18B20_write(0xFF);
		DS18B20_write(0x7F);
	}
	
	LED_fadeout();
	
    while(1)
    {
	    LED_fadein();
		
		DS18B20_start_meas();
		_delay_ms(1000);
		DS_temp = DS18B20_temperature();
		
		DHT11_getData(&DHT_temp, &DHT_hum);
		
		BMP180_calculation(&BMP_temp, &BMP_press);
		
		if(DS_temp >= 0) {
			temp = DS_temp;
		}
		else {
			temp = (-1) * DS_temp;
		}
		buf[33] = temp % 10 + 0x30;
		temp /= 10;
		buf[32] = temp % 10 + 0x30;
		temp /= 10;
		buf[30] = temp % 10 + 0x30;
		temp /= 10;
		buf[29] = temp % 10 + 0x30;
		if(DS_temp < 0) {
			buf[28] = '-';
		}
		else {
			buf[28] = '0';
		}
		
		if(DHT_temp >= 0) {
			temp = DHT_temp;
		}
		else {
			temp = (-1) * DHT_temp;
		}
		buf[46] = temp % 10 + 0x30;
		temp /= 10;
		buf[45] = temp % 10 + 0x30;
		if(DS_temp < 0) {
			buf[44] = '-';
		}
		else {
			buf[44] = '0';
		}
		
		temp = DHT_hum;
		buf[58] = temp % 10 + 0x30;
		temp /= 10;
		buf[57] = temp % 10 + 0x30;
		
		if(BMP_temp >= 0) {
			temp = BMP_temp;
		}
		else {
			temp = (-1) * BMP_temp;
		}
		buf[74] = temp % 10 + 0x30;
		temp /= 10;
		buf[72] = temp % 10 + 0x30;
		temp /= 10;
		buf[71] = temp % 10 + 0x30;
		if(DS_temp < 0) {
			buf[70] = '-';
		}
		else {
			buf[70] = '0';
		}
		
		temp = BMP_press;
		buf[90] = temp % 10 + 0x30;
		temp /= 10;
		buf[88] = temp % 10 + 0x30;
		temp /= 10;
		buf[87] = temp % 10 + 0x30;
		temp /= 10;
		buf[86] = temp % 10 + 0x30;
		
	    UART_send_string("AT+CIPSTART=\"TCP\",\"narodmon.ru\",8283\r\n");
	    _delay_ms(100);
	    //UART_send_string("AT+CIPSEND=94\r\n");
	    UART_send_string("AT+CIPSEND=62\r\n");
	    _delay_ms(100);
	    UART_send_string(buf);
		
		LED_fadeout();
		
		for(int i = 0; i < 5; i++) {
			_delay_ms(60000);
		}
    }
}