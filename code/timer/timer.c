#include "timer.h"
#include "N76E003.h"
#include "typedef.h"
#include "led.h"
#include "key_driver.h"

void timerInit(void)
{
	TMOD &= 0xf0;
	TMOD |= 0X02; //开启定时器0模式2
	CKCON &= 0xf7; //定时器时钟为1/12系统时钟
	TH0 = 123;   //100us
	TL0 = 123;
	ET0 = 1;
	EA = 1;
	TR0 = 1;
}

void Timer0_ISR(void) interrupt 1
{
	//100us的自动重装定时器
	static uint16_t cnt = 0;
	static uint16_t cnt1 = 0;
	cnt++;
	cnt1++;
   	if(cnt >= 100)
	{
		cnt = 0;	
		keyScan();
	}
	if(cnt1 >= 5000)
	{
		cnt1 = 0;
		RF_LED = ~RF_LED;
	}
	
}