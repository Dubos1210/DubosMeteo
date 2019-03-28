/*
 * DHT11.h
 *
 * Created: 21.03.2019 15:24:42
 *  Author: Vladimir
 */ 


#ifndef DHT11_H_
#define DHT11_H_

#define DHT_WIRE		6
#define DHT_DDR			DDRD
#define DHT_PORT		PORTD
#define DHT_PIN			PIND

#define DHT11_OK		0xFF
#define DHT11_ERROR		0x00

uint8_t DHT11_getData(int8_t* temperature, uint8_t* humidity);
uint8_t DHT11_readByte(void);

#endif /* DHT11_H_ */