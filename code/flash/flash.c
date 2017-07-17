#include "flash.h"
#include <stdio.h>
#include "N76E003.h"
#include "typedef.h"

#if 0
bit BIT_TMP;
#define set_IAPEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON|=0x01 ;EA=BIT_TMP;
#define set_APUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN|=0x01;EA=BIT_TMP
#define set_IAPGO   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPTRG|=0x01 ;EA=BIT_TMP
#define clr_APUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN&=~0x01;EA=BIT_TMP
#define clr_IAPEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON&=~0x01;EA=BIT_TMP;

uint8_t read_APROM_BYTE(uint16_t code *u16_addr)
{
	uint8_t rdata;
	rdata = *u16_addr>>8;
	return rdata;
}

void write_DATAFLASH_BYTE(uint16_t u16_addr,uint8_t u8_data)
{
	uint8_t looptmp=0,u8_addrl_r;
	uint16_t code *cd_longaddr;
	unsigned char xdata *xd_tmp;
	
//Check page start address
	u8_addrl_r = u16_addr;
	if (u8_addrl_r<0x80)
	{
		u8_addrl_r = 0;
	}
	else 
	{
		u8_addrl_r = 0x80;
	}
//Save APROM data to XRAM
	xd_tmp = 0x80;
	cd_longaddr = (uint16_t code*)((u16_addr&0xff00)+u8_addrl_r);	
	while (xd_tmp !=0x100)
	{
		*xd_tmp = (uint8_t)(*cd_longaddr);
		looptmp++;
		xd_tmp++;
		cd_longaddr++;
	}
// Modify customer data in XRAM
	u8_addrl_r = u16_addr;
	if (u8_addrl_r<0x80)
	{
		xd_tmp = u8_addrl_r+0x80;
	}
	else
	{
		xd_tmp = u8_addrl_r+0;
	}
	*xd_tmp = u8_data;
//Erase APROM DATAFLASH page
    IAPAL = u16_addr;
    IAPAH = u16_addr>>8;
    IAPFD = 0xFF;
    set_IAPEN; 
    set_APUEN;
    IAPCN = 0x22; 		
    set_IAPGO; 
//Save changed RAM data to APROM DATAFLASH
	u8_addrl_r = u16_addr;
	if (u8_addrl_r<0x80)
	{
		u8_addrl_r =0;
	}
	else
	{
		u8_addrl_r = 0x80;
	}
    xd_tmp = 0x80;
    IAPAL = u8_addrl_r;
    IAPAH = u16_addr>>8;
    set_IAPEN; 
    set_APUEN;
    IAPCN = 0x21;
    while (xd_tmp !=0xFF)
    {
        IAPFD = *xd_tmp;
        set_IAPGO;
        IAPAL++;
        xd_tmp++;
    }
    clr_APUEN;
    clr_IAPEN;
}	
#endif

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
	IAPAH = 0x38;
	IAPAL = 0x00;
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
		IAPAH = 0X38;
		IAPAL = i;
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
		flash_data[i] = *(uint16_t code *)(0x3800 + i)>>8;
		//flash_data[i] = Data_Flash[i];
	}		
}


void flashWrite(void)
{
	flashEnableAprom();
	flashErase();
	flashWriteBytes();
	flashDisableAprom();	
}


