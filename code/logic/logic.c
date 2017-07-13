#include <intrins.h>
#include "logic.h"
#include "N76E003.h"
#include "key.h"
#include "led.h"
#include "timer.h"
#include "key_driver.h"
#include "flash.h"
#include "remote.h"

sbit RCK = P1^7;
sbit SCK = P1^6;
sbit SER = P3^0;

bool led_blink_flag = false;
bool key_scan_flag = false;
bool update_local_flag = false;
bool deal_jogging = false;
bool deal_lock = false;
bool syn_app_flag = false;
bool save_mode_flag = false;
uint8_t update_time = 5;

uint8_t code relay_array[4] = {0x80,0x40,0x20,0x10};
uint8_t code key_array[4] = {0x04,0x02,0x01,0x08};

DevDef xdata dev_def = {0};
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
static void syncApp(void)
{
    uint8_t last_key = h595_val;
    uint8_t relays[4] = {0};
    uint8_t i = 0;
    relays[0] = RELAY1;
    relays[1] = RELAY2;
    relays[2] = RELAY3;
    relays[3] = RELAY4;
    if(dev_def.lock)
    {
        static uint8_t lock_channel = 0;
        static uint8_t last_lock_channel = 0;
        static uint8_t last_relay_val[4] = {0};
        if(dev_def.dev_channel[0].update_flag 
            || dev_def.dev_channel[1].update_flag 
            || dev_def.dev_channel[2].update_flag
            || dev_def.dev_channel[3].update_flag)
        {
            return;
        }
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
            last_lock_channel = lock_channel;
            for(i = 0; i < 4; i++)
            {
                if(lock_channel == (i + 1))
                {
                    h595_val &= 0x0f;
                    h595_val |= relay_array[i];
                }
            }
            dev_def.dev_channel[0].update_flag = true;
            dev_def.dev_channel[1].update_flag = true;
            dev_def.dev_channel[2].update_flag = true;
            dev_def.dev_channel[3].update_flag = true;
            dev_def.update_local_cnt = 0;
            dev_def.setting = false;
            update_time = 5;
        }
        else
        {
            if(relays[lock_channel - 1])
            {
                if(!(h595_val & relay_array[lock_channel - 1]))
                {
                    h595_val |= relay_array[lock_channel - 1];
                    dev_def.dev_channel[0].update_flag = true;
                    dev_def.dev_channel[1].update_flag = true;
                    dev_def.dev_channel[2].update_flag = true;
                    dev_def.dev_channel[3].update_flag = true;
                    dev_def.update_local_cnt = 0;
                    dev_def.setting = false;
                    update_time = 5;
                }
               
            }
            else
            {
                if(h595_val & relay_array[lock_channel - 1])
                {
                    h595_val &= ~relay_array[lock_channel - 1];
                    dev_def.dev_channel[0].update_flag = true;
                    dev_def.dev_channel[1].update_flag = true;
                    dev_def.dev_channel[2].update_flag = true;
                    dev_def.dev_channel[3].update_flag = true;
                    dev_def.update_local_cnt = 0;
                    dev_def.setting = false;
                    update_time = 5;
                }
            }
        }
    }
    else
    {
        for(i =  0; i < 4; i++)
        {
            if(dev_def.dev_channel[i].channel_mode == DEV_JOGGING)
            {
                dev_def.dev_channel[i].timer_cnt++;
                if((h595_val & relay_array[i]))//如果relay1是按下的
                {
                    if(dev_def.dev_channel[i].get_status == false)
                    {
                        dev_def.dev_channel[i].timer_cnt = 0;
                        dev_def.dev_channel[i].get_status = true;
                    }
                    if(dev_def.dev_channel[i].timer_cnt >= 7)
                    {
                        dev_def.dev_channel[i].timer_cnt = 0;
                        dev_def.dev_channel[i].get_status = false;
                        h595_val &= (~relay_array[i]);//关掉relay1
                        dev_def.dev_channel[i].update_flag = true;//允许更新标志位
                        dev_def.update_local_cnt = 0;
                        dev_def.setting = false;
                        update_time = 5;
                    }
                }
            }
            if(!dev_def.dev_channel[i].update_flag)
            {
                if(relays[i])
                {
                    h595_val |= relay_array[i];
                }
                else
                {
                    h595_val &= (~relay_array[i]);
                }
            }
        }
    }
    if(h595_val != last_key)
    {
        SendTo595(h595_val);
    }
}
static void updateLocal(void)
{
    static bool high_flag[4] = {false};
    uint8_t i = 0;
    uint8_t last_key = h595_val;
    uint8_t relays[4] = {0};
    dev_def.update_local_cnt++;
    if(dev_def.update_local_cnt == update_time)
    {
        relays[0] = (uint8_t)RELAY1;
        relays[1] = (uint8_t)RELAY2;
        relays[2] = (uint8_t)RELAY3;
        relays[3] = (uint8_t)RELAY4;
        for(i = 0; i < 4; i++)
        {
            if(dev_def.dev_channel[i].update_flag)
            {
                if((relays[i] << (7 - i)) != (h595_val & relay_array[i]))
                {
                    high_flag[i] = true;
                    h595_val &= ~key_array[i];
                }
                else
                {
                    dev_def.dev_channel[i].update_flag = false;
                }
            }
        }
        if(h595_val != last_key)
        {
            SendTo595(h595_val);
        }
    }
    else if(dev_def.update_local_cnt >= update_time + 1)
    {
        dev_def.update_local_cnt = 0;
        for(i = 0; i < 4; i++)
        {
            if(high_flag[i] == true)
            {
                h595_val |= key_array[i];
                high_flag[i] = false;
            }
        }
        if(h595_val != last_key)
        {
            SendTo595(h595_val);
        }
    }

    
}
void dealLogic(void)
{
    if(key_scan_flag)
    {
        key_scan_flag = false;
        keyScan();
    }
    if(syn_app_flag)
    {
        syn_app_flag = false;
        syncApp();
    }
    if(update_local_flag)
    {
        update_local_flag = false;
        updateLocal();//要在同步APP之后
    }
}
void saveModeToFlash(void)
{
    if(save_mode_flag)
    {
        save_mode_flag = false;
        write_DATAFLASH_BYTE (0x3881,dev_def.lock);
        write_DATAFLASH_BYTE (0x3882,(uint8_t)dev_def.dev_channel[0].channel_mode);
        write_DATAFLASH_BYTE (0x3883,(uint8_t)dev_def.dev_channel[1].channel_mode);
        write_DATAFLASH_BYTE (0x3884,(uint8_t)dev_def.dev_channel[2].channel_mode);
        write_DATAFLASH_BYTE (0x3885,(uint8_t)dev_def.dev_channel[3].channel_mode);
    }
}
static void modeInit(void)
{
    dev_def.lock = read_APROM_BYTE(0x3881);
    dev_def.dev_channel[0].channel_mode = read_APROM_BYTE(0x3882);
    dev_def.dev_channel[1].channel_mode = read_APROM_BYTE(0x3883);
    dev_def.dev_channel[2].channel_mode = read_APROM_BYTE(0x3884);
    dev_def.dev_channel[3].channel_mode = read_APROM_BYTE(0x3885);
    if(dev_def.lock == 0xff 
        || dev_def.dev_channel[0].channel_mode == 0xff
        || dev_def.dev_channel[1].channel_mode == 0xff
        || dev_def.dev_channel[2].channel_mode == 0xff
        || dev_def.dev_channel[3].channel_mode == 0xff)
    {
        dev_def.lock = false;
        dev_def.dev_channel[0].channel_mode = DEV_SELFLOCK;
        dev_def.dev_channel[1].channel_mode = DEV_SELFLOCK;
        dev_def.dev_channel[2].channel_mode = DEV_SELFLOCK;
        dev_def.dev_channel[3].channel_mode = DEV_SELFLOCK;
    }
    if(dev_def.lock)
    {
        MODE_LED = 1;
    }
    else
    {
        MODE_LED = 0;
    }
}
void logicInit(void)
{
    modeInit();
    h595Init();
    SendTo595(0x0f);
    ledInit();   
	keyInit();
    relayInit();
//	captureInit();
//	timer1Init();
    timer0Init();
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


