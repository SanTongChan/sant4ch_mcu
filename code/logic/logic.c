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
bool deal_jogging = false;
DevDef dev_def = {0};

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
#if 0
    uint8_t temp = 0;
    temp = (((uint8_t)RELAY1 << 7) | ((uint8_t)RELAY2 << 6) | ((uint8_t)RELAY3 << 5) | ((uint8_t)RELAY4 << 4));
    if((temp & 0xf0) != (h595_val & 0xf0))
    {
        h595_val = ((h595_val & 0x0f) | (temp & 0xf0));
        SendTo595(h595_val);
    }
#endif
    uint8_t last_val = 0;
    last_val = h595_val;
    if((((uint8_t)RELAY1 << 7) != (h595_val & RELAY1_595)) && (!dev_def.dev_channel[0].update_flag))
    {
        h595_val &= (~RELAY1_595);
        h595_val |= ((uint8_t)RELAY1 << 7);
    }
    if((((uint8_t)RELAY2 << 6) != (h595_val & RELAY2_595)) && (!dev_def.dev_channel[1].update_flag))
    {
        h595_val &= (~RELAY2_595);
        h595_val |= ((uint8_t)RELAY2 << 6);
    }
    if((((uint8_t)RELAY3 << 5) != (h595_val & RELAY3_595)) && (!dev_def.dev_channel[2].update_flag))
    {
        h595_val &= (~RELAY3_595);
        h595_val |= ((uint8_t)RELAY3 << 5);
    }
    if((((uint8_t)RELAY4 << 4) != (h595_val & RELAY4_595)) && (!dev_def.dev_channel[3].update_flag))
    {
        h595_val &= (~RELAY4_595);
        h595_val |= ((uint8_t)RELAY4 << 4);
    }
    if(last_val != h595_val)
    {
        SendTo595(h595_val);
    }
}
static void dealJogging(void)
{
    static uint16_t cnt = 0;
    //static bool lock = false;
    cnt++;
    if(!dev_def.lock)
    {
        MODE_LED = 1;
        dev_def.dev_channel[0].timer_cnt++;
        if(dev_def.dev_channel[0].channel_mode == DEV_JOGGING)
        {
            dev_def.dev_channel[0].timer_cnt++;
            if((h595_val & RELAY1_595))//如果relay1是按下的
            {
                if(dev_def.dev_channel[0].get_status == false)
                {
                    dev_def.dev_channel[0].timer_cnt = 0;
                    dev_def.dev_channel[0].get_status = true;
                }
                if(dev_def.dev_channel[0].timer_cnt >= 100)
                {
                    dev_def.dev_channel[0].timer_cnt = 0;
                    dev_def.dev_channel[0].get_status = false;
                    h595_val &= (~RELAY1_595);//关掉relay1
                    dev_def.dev_channel[0].update_flag = true;//允许更新标志位
                    cnt = 0;
                }
            }
            else
            {
                dev_def.dev_channel[0].timer_cnt = 0;
                if(RELAY1 == 0 && dev_def.dev_channel[0].update_flag == true)
                {
                    h595_val |= KEY1_595;
                    dev_def.dev_channel[0].update_flag = false;
                }
            }
            if(cnt == 50)
            {
                if(dev_def.dev_channel[0].update_flag == true)
                {
                    h595_val &= ~KEY1_595;
                }
            }
            else if(cnt > 60 && cnt <= 70)
            {
                if(dev_def.dev_channel[0].update_flag == true)
                {
                    if((h595_val & KEY1_595) == 0)
                    {
                         h595_val |= KEY1_595;
                    }
                }
            }
            else if(cnt > 70)
            {
                if(((uint8_t)RELAY1 << 7) == (h595_val & RELAY1_595))
                {
                    cnt = 0;
                    dev_def.dev_channel[0].update_flag = false;
                }
            }
        }
        /*
        else
        {
            if((h595_val & KEY1_595) || dev_def.dev_channel[0].update_flag)
            {
                h595_val |= KEY1_595;
                dev_def.dev_channel[0].update_flag = false;
            }
        }
        */
        SendTo595(h595_val);
    }
    else
    {

    }
}
void dealLogic(void)
{
    if(key_scan_flag)
    {
        key_scan_flag = false;
        keyScan();
    }
    
    if(deal_jogging)
    {
        dealJogging();
        deal_jogging = false;
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
    timer0Init();
    dev_def.dev_channel[0].channel_mode = DEV_JOGGING;
    MODE_LED = 0;
}
void SendTo595(uint8_t val)
{
	char i=0;
	uint8_t temp = val;
	EA = 0;
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
	EA = 1;
}


