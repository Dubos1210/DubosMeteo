/*
 * DS18B20.h
 *
 * Created: 28.12.2018 14:32:14
 *  Author: Vladimir
 */ 


#ifndef DS18B20_H_
#define DS18B20_H_

#include <stdint.h>

#define DQ_in() DDRD &=~ (1<<6) //DQ - вход (высокий уровень на шине благодаря внешней подтяжке)
#define DQ_out() DDRD |= (1<<6) //DQ - выход
#define DQ_pin() PIND & (1<<6)  //Проверка состояния DQ

#define SKIP_ROM         0xCC
#define CONVERT          0x44
#define WRITE_SCRATCHPAD 0x4E
#define READ_SCRATCHPAD  0xBE

uint8_t DS18B20_rst(void);
void DS18B20_write(uint8_t ds18b20_data);
uint8_t DS18B20_read(void);
int16_t DS18B20_temperature(void);
void DS18B20_start_meas(void);

#endif /* DS18B20_H_ */