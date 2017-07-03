#include "flash.h"
#include <stdio.h>
#include "N76E003.h"
#include "typedef.h"

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
	unsigned char code *cd_longaddr;
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
	cd_longaddr = (u16_addr&0xff00)+u8_addrl_r;	
	while (xd_tmp !=0x100)
	{
		*xd_tmp = *cd_longaddr;
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

#if 0
void main (void) 
{
    UINT8 datatemp;
    write_DATAFLASH_BYTE (0x3881,0x55);
    write_DATAFLASH_BYTE (0x3882,0x56);
    write_DATAFLASH_BYTE (0x3855,0xaa);
    write_DATAFLASH_BYTE (0x3856,0x66);
    datatemp = read_APROM_BYTE(0x3882);
    while(1);
}
#endif
