#ifndef __FLASH_H__
#define __FLASH_H__

#include "typedef.h"

uint8_t read_APROM_BYTE(uint16_t code *u16_addr);
void write_DATAFLASH_BYTE(uint16_t u16_addr,uint8_t u8_data);

#endif
