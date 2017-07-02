#include "timer.h"
#include "N76E003.h"
#include "typedef.h"
#include "led.h"
#include "logic.h"

void timer0Init(void)
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
	static uint16_t key_cnt = 0;
	static uint16_t update_cnt = 0;
	static uint16_t led_cnt = 0;
	static uint16_t update_threod = 0;
	key_cnt++;
	update_cnt++;
	led_cnt++;
	if(key_cnt >= 100)
	{
	    key_cnt = 0;
        key_scan_flag = true;
        deal_relay = true;
	}
	if(update_cnt >= 15000 - update_threod)
	{
        update_threod = 13000;
        update_status_flag = true;
	}
	if(led_cnt >= 5000)
	{
        led_cnt = 0;
        RF_LED = ~RF_LED;
	}
}