#include <intrins.h>
#include "logic.h"
#include "N76E003.h"
#include "key.h"
#include "led.h"
#include "timer.h"
#include "key_driver.h"

sbit RCK = P1^7;
sbit SCK = P1^6;
sbit SER = P3^0;
bool key_scan_flag = false;
bool update_status_flag = false;
bool led_blink_flag = false;

static void h595Init(void)
{
    //初始化P16 P17 P30为推挽输出
    P1M1 &= 0x3f;
   	P1M2 |= 0Xc0;
   	P3M1 &= 0xfe;
    P3M2 |= 0x01;
    SER = 1;
}
static void relayInit(void)
{
    P0M1 |= 0x0f;//初始化relay脚 P00 -> P03	为输入
   	P0M2 &= 0Xf0;  
}
static void updateDeviceStatus(void)
{
    uint8_t temp = 0;
    temp = (((uint8_t)RELAY1 << 7) | ((uint8_t)RELAY2 << 6) | ((uint8_t)RELAY3 << 5) | ((uint8_t)RELAY4 << 4));
    if((temp & 0xf0) != (h595_val & 0xf0))
    {
        h595_val = ((h595_val & 0x0f) | (temp & 0xf0));
        SendTo595(h595_val);
    }
}
void dealLogic(void)
{
    if(key_scan_flag)
    {
        key_scan_flag = false;
        keyScan();
    }
    if(update_status_flag)
    {
        updateDeviceStatus();
        update_status_flag = false;
    }
}
void logicInit(void)
{
    h595Init();
    SendTo595(0x0f);
    ledInit();   
	keyInit();
    relayInit();
    timerInit();
}
void SendTo595(uint8_t val)
{
	char i=0;
	uint8_t temp = val;
	for(i = 0; i < 8; i++)
	{
		SER = temp>>7;
		temp= temp<<1;
		SCK = 0;
		_nop_();
		SCK = 1;
	}
	RCK = 0;
	_nop_();
	RCK = 1;
	SER = 0;
}


