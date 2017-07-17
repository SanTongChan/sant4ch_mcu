#include "flash.h"
#include <stdio.h>
#include "N76E003.h"
#include "typedef.h"

#define PAGE_ERASE_AP  (0X22)
#define BYTE_PROGRAM_AP (0x21)
uint8_t xdata flash_data[FLASH_MAX_LEN] = {0xff};

static void flashEnableAprom(void)
{
	TA = 0xAA; //CHPCON is TA protected
	TA = 0x55;
	CHPCON |= 0x01; //IAPEN = 1, enable IAP mode
	TA = 0xAA; //IAPUEN is TA protected
	TA = 0x55;
	IAPUEN |= 0x01; //APUEN = 1, enable APROM update
}


static void flashErase(void)
{
	IAPCN = PAGE_ERASE_AP; //Erase page 
	IAPAH = FLASH_START_ADDR_H;
	IAPAL = FLASH_START_ADDR_L;
	IAPFD = 0xFF;
	TA = 0xAA; //IAPTRG is TA protected
	TA = 0x55;
	IAPTRG |= 0x01; //write ¡®1¡¯ to IAPGO to trigger IAP process
}

static void flashDisableAprom(void)
{
	TA = 0xAA; //IAPUEN is TA protected
	TA = 0x55;
	IAPUEN &= ~0x01; //APUEN = 0, disable APROM update
	TA = 0xAA; //CHPCON is TA protected
	TA = 0x55;
	CHPCON &= ~0x01; //IAPEN = 0, disable IAP mode	
}

static void flashWriteBytes(void)
{
#if 1
	uint8_t i = 0;	
	for(i = 0; i < FLASH_MAX_LEN; i++)
	{
		IAPCN = BYTE_PROGRAM_AP;
		IAPAH = FLASH_START_ADDR_H;
		IAPAL = FLASH_START_ADDR_L + i;
		IAPFD = flash_data[i];
		TA = 0XAA;
		TA = 0X55;
		IAPTRG |= 0X01;
	}	
#endif
}
void flashRead(void)
{
	unsigned char i = 0;
	for(i = 0; i < FLASH_MAX_LEN; i++)
	{
		flash_data[i] = *(uint16_t code *)(FLASH_START_ADDR_BASE + i)>>8;
	}		
}


void flashWrite(void)
{
	flashEnableAprom();
	flashErase();
	flashWriteBytes();
	flashDisableAprom();	
}


