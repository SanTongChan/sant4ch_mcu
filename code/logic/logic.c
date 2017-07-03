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
bool lock_flag = false;
uint8_t relay_array[4] = {0x80,0x40,0x20,0x10};
uint8_t key_array[4] = {0x04,0x02,0x01,0x08};

DevDef dev_def = {0};
uint16_t jogging_cnt = 0;
static void h595Init(void)
{
    //��ʼ��P16 P17 P30Ϊ�������
    P1M1 &= 0x3f;
   	P1M2 |= 0Xc0;
   	P3M1 &= 0xfe;
    P3M2 |= 0x01;
    SER = 1;
}
static void relayInit(void)
{
    P0M1 |= 0x0f;//��ʼ��relay�� P00 -> P03	Ϊ����
   	P0M2 &= 0Xf0;  
}
static void updateDeviceStatus(void)
{
    uint8_t last_val = 0;
    if(!dev_def.lock)
    {
        last_val = h595_val;
        if((((uint8_t)RELAY1 << 7) != (h595_val & RELAY1_595)) && (!dev_def.dev_channel[0].update_flag))
        {
            h595_val &= (~RELAY1_595);
            h595_val |= ((uint8_t)RELAY1 << 7);
        }
        if((((uint8_t)RELAY2 << 6) != (h595_val & RELAY2_595)) && (!dev_def.dev_channel[1].update_flag))
        {   
            h595_val = (~RELAY2_595);
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
}
static void dealJogging(void)
{
    uint8_t relays[4] = {0};
    uint8_t i = 0;
    uint8_t last_val = 0;
    if(!dev_def.lock)
    {
        jogging_cnt++;
        last_val = h595_val;
        relays[0] = (uint8_t)RELAY1;
        relays[1] = (uint8_t)RELAY2;
        relays[2] = (uint8_t)RELAY3;
        relays[3] = (uint8_t)RELAY4;
        for(i =  0; i < 4; i++)
        {
            if(dev_def.dev_channel[i].channel_mode == DEV_JOGGING)
            {
                dev_def.dev_channel[i].timer_cnt++;
                if((h595_val & relay_array[i]))//���relay1�ǰ��µ�
                {
                    if(dev_def.dev_channel[i].get_status == false)
                    {
                        dev_def.dev_channel[i].timer_cnt = 0;
                        dev_def.dev_channel[i].get_status = true;
                    }
                    if(dev_def.dev_channel[i].timer_cnt >= 100)
                    {
                        dev_def.dev_channel[i].timer_cnt = 0;
                        dev_def.dev_channel[i].get_status = false;
                        h595_val &= (~relay_array[i]);//�ص�relay1
                        dev_def.dev_channel[i].update_flag = true;//������±�־λ
                        jogging_cnt = 0;
                    }
                }
                else
                {
                    dev_def.dev_channel[i].timer_cnt = 0;
                    if(relays[i] == 0 && dev_def.dev_channel[i].update_flag == true)
                    {
                        h595_val |= key_array[i];
                        dev_def.dev_channel[i].update_flag = false;
                    }
                }
            }
        }
        if(jogging_cnt == 80)
        {
            for (i = 0; i < 4; i++)
            {
                if((dev_def.dev_channel[i].update_flag == true) 
                    && ((h595_val & key_array[i]))
                    && (!dev_def.dev_channel[i].lock))
                {
                    h595_val &= ~key_array[i];
                }
            }
        }
        else if(jogging_cnt == 100)
        {
            for(i = 0; i < 4; i++)
            {
                if((dev_def.dev_channel[i].update_flag == true)
                    && ((h595_val & key_array[i]) == 0)
                    && (!dev_def.dev_channel[i].lock))
                {
                    //MODE_LED = 1;
                    h595_val |= key_array[i];
                    dev_def.dev_channel[i].lock = true;
                }
            }
        }
        for(i = 0; i < 4; i++)
        {
            if((relays[i] << (7 - i)) == (h595_val & relay_array[i])
                && dev_def.dev_channel[i].lock == true)
            {
                dev_def.dev_channel[i].lock = false;
                dev_def.dev_channel[i].update_flag = false;
            }
        }    
        if(last_val != h595_val)
        {
            SendTo595(h595_val);
        }
    }
}
static void dealLock(void)
{
    static uint8_t lock_channel = 0;
    static uint8_t last_lock_channel = 0;
    static uint8_t last_relay_val[4] = {0};
    uint8_t relays[4] = {0};
    uint8_t i = 0;
    uint8_t last_val = h595_val;
    if(dev_def.lock)
    {
        dev_def.lock_cnt++;
        relays[0] = RELAY1;
        relays[1] = RELAY2;
        relays[2] = RELAY3;
        relays[3] = RELAY4;
        for(i = 0; i < 4;i++)
        {
            if(relays[i] != last_relay_val[i] && relays[i] == 1)
            {
                lock_channel = i + 1;
            }
            last_relay_val[i] = relays[i];
        }
        if(last_lock_channel != lock_channel)
        {
            MODE_LED = !MODE_LED;
            last_lock_channel = lock_channel;
            for(i = 0; i < 4; i++)
            {
                if(relays[i] == 1 && lock_channel != (i + 1))
                {
                    h595_val &= ~key_array[i];
                    h595_val &= ~relay_array[i];
                }
                else if(lock_channel == (i + 1))
                {
                    if(relays[i])
                    {
                        if(!(h595_val & relay_array[i]))
                        {
                            h595_val |= relay_array[i];
                        }
                    }
                }
            }
            dev_def.lock_cnt = 0;
            dev_def.lock_update = true;
        }
        else
        {
            
            if(relays[lock_channel - 1])
            {
                if(!(h595_val & relay_array[lock_channel - 1]))
                {
                    h595_val |= relay_array[lock_channel - 1];
                }
               
            }
            else
            {
                if(h595_val & relay_array[lock_channel - 1])
                {
                    h595_val &= ~relay_array[lock_channel - 1];
                }
            }
        }
#if 1
        if(dev_def.lock_cnt >= 50 && dev_def.lock_update)
        {
            dev_def.lock_cnt = 0;
            dev_def.lock_update = false;
            h595_val |= 0x0f;
        }
        if(h595_val != last_val)
        {
            SendTo595(h595_val);
        }
#endif
    }
}
/*
static void dealJogging(void)
{
    uint8_t last_val = 0;
    jogging_cnt++;
    last_val = h595_val;
    if(!dev_def.lock)
    {
        dealJogging();
    }
    if(last_val != h595_val)
    {
        SendTo595(h595_val);
    }
}
*/
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
    if(lock_flag)
    {
        lock_flag = false;
        dealLock();
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
    dev_def.dev_channel[1].channel_mode = DEV_JOGGING;
    dev_def.dev_channel[2].channel_mode = DEV_JOGGING;
    dev_def.dev_channel[3].channel_mode = DEV_JOGGING;
    dev_def.lock = true;
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


