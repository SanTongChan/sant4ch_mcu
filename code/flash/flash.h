#ifndef __FLASH_H__
#define __FLASH_H__

#include "typedef.h"

#define FLASH_MAX_LEN     (10)
#define FLASH_START_ADDR   (0x3800)
extern uint8_t xdata flash_data[FLASH_MAX_LEN];

void flashRead(void);
void flashWrite(void);


//uint8_t read_APROM_BYTE(uint16_t code *u16_addr);
//void write_DATAFLASH_BYTE(uint16_t u16_addr,uint8_t u8_data);

#endif
