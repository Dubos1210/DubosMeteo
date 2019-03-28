/*
 * BMP180.h
 *
 * Created: 29.12.2018 22:38:27
 *  Author: Vladimir
 */ 


#ifndef BMP180_H_
#define BMP180_H_

#define BMP180_ADDR 0xEE // 7-bit address

#define BMP180_DEVICE_ID	0xD0
#define BMP180_ID			0x55

#define	BMP180_REG_CONTROL 0xF4
#define	BMP180_REG_RESULT 0xF6

#define	BMP180_COMMAND_TEMPERATURE 0x2E
#define	BMP180_COMMAND_PRESSURE0 0x34
#define	BMP180_COMMAND_PRESSURE1 0x74
#define	BMP180_COMMAND_PRESSURE2 0xB4
#define	BMP180_COMMAND_PRESSURE3 0xF4

uint8_t BMP180_init(void);
void BMP180_calculation (int32_t* temperature, int32_t* pressure);


#endif /* BMP180_H_ */