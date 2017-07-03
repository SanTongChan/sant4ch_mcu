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
    static uint16_t key_cnt = 0;
    static uint16_t update_local_cnt = 0;
    static uint16_t syn_app_cnt = 0;
    static uint16_t jogging_cnt = 0;
    static uint16_t lock_cnt = 0;
    static uint16_t update_threod = 0;
    static uint16_t syn_threod = 0;
    static uint16_t jogging_threod = 0;
    static uint16_t lock_threod = 0;
    static uint16_t led_cnt = 0;
    key_cnt++;
    update_local_cnt++;
    syn_app_cnt++;
    jogging_cnt++;
    lock_cnt++;
    led_cnt++;
    if(key_cnt >= 100)
    {
        key_cnt = 0;
        key_scan_flag = true;
    }
    if(update_local_cnt >= 10000 - update_threod)
    {
        update_threod = 9000;
        update_local_cnt = 0;
        update_local_flag = true;
    }
    if(syn_app_cnt >= 10000 - syn_threod)
    {
        syn_threod = 9000;
        syn_app_cnt = 0;
        syn_app_flag = true;
    }
    if(jogging_cnt >= 10000 - jogging_threod)
    {
        jogging_threod = 9900;
        jogging_cnt = 0;
        deal_jogging = true;
    }
    if(lock_cnt >= 10000 - lock_threod)
    {
        lock_threod = 9900;
        lock_cnt = 0;
        deal_lock = true;
    }
    if(led_cnt >= 5000)
	{
        led_cnt = 0;
        RF_LED = ~RF_LED;
	}
}