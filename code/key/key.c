#include "key.h"
#include "key_driver.h"
#include "led.h"
#include "logic.h"

sbit KEY1 = P1^3;
sbit KEY2 = P1^2;
sbit KEY3 = P1^1;
sbit KEY4 = P1^0;
sbit KEY5 = P1^4;
uint8_t h595_val = 0x0f;

static bool key1_is_press(void)
{
    if(0 == KEY1)
    {
        return true;
    }
    return false;
}
static bool key2_is_press(void)
{
    if(0 == KEY2)
    {
        return true;
    }
    return false;
}
static bool key3_is_press(void)
{
    if(0 == KEY3)
    {
        return true;
    }
    return false;
}
static bool key4_is_press(void)
{
    if(0 == KEY4)
    {
        return true;
    }
    return false;
}
static bool key5_is_press(void)
{
    if(0 == KEY5)
    {
        return true;
    }
    return false;
}

static void key1_short_press(void)
{
    if(dev_def.remote)
    {
        dev_def.remote_channel = 1;
    }
    else
    {
        if(h595_val | KEY1_595)
        {
            h595_val = h595_val & (~KEY1_595);
    	    SendTo595(h595_val);
        }
    }
}
static void key2_short_press(void)
{
    if(dev_def.remote)
    {
        dev_def.remote_channel = 2;
    }
    else
    {
        if(h595_val | KEY2_595)
        {
            h595_val = h595_val & (~KEY2_595);
        	SendTo595(h595_val);	
    	}
	}
}
static void key3_short_press(void)
{   
    if(dev_def.remote)
    {
        dev_def.remote_channel = 3;
    }
    else
    {
        if(h595_val | KEY3_595)
        {
        	h595_val = h595_val & (~KEY3_595);
        	SendTo595(h595_val);
    	}
	}
}
static void key4_short_press(void)
{
    if(dev_def.remote)
    {
        dev_def.remote_channel = 4;
    }
    else
    {
        if(h595_val | KEY4_595)
        {
        	h595_val = h595_val & (~KEY4_595);
        	SendTo595(h595_val);
    	}
	}
}
static void key5_short_up_press(void)
{
    if(dev_def.remote)
    {
        dev_def.remote = false;
    }
}

static void key1_short_up_press(void)
{
    if(!dev_def.remote)
    {
        if(RELAY1)
        {
            h595_val &= (~RELAY1_595);
        }
        else
        {
            if(dev_def.lock)
            {
                h595_val &= 0x0f;
            }
            h595_val |= RELAY1_595;
        }
        h595_val |= KEY1_595;
        SendTo595(h595_val);
        if(dev_def.setting == true)
        {
            dev_def.setting = false;
            update_time = 10;
        }
        else
        {
            update_time = 5;
        }
        dev_def.dev_channel[0].update_flag = true;
        dev_def.dev_channel[1].update_flag = true;
        dev_def.dev_channel[2].update_flag = true;
        dev_def.dev_channel[3].update_flag = true;
        dev_def.update_local_cnt = 0;
    }
}
static void key2_short_up_press(void)
{
    if(!dev_def.remote)
    {
      	if(RELAY2)
        {
            h595_val &= (~RELAY2_595);
        }
        else
        {
            if(dev_def.lock)
            {
                h595_val &= 0x0f;
            }
            h595_val |= RELAY2_595; 
        }
        h595_val |= KEY2_595;
        SendTo595(h595_val);
        if(dev_def.setting == true)
        {
            dev_def.setting = false;
            update_time = 10;
        }
        else
        {
            update_time = 5;
        }
        dev_def.dev_channel[0].update_flag = true;
        dev_def.dev_channel[1].update_flag = true;
        dev_def.dev_channel[2].update_flag = true;
        dev_def.dev_channel[3].update_flag = true;
        dev_def.update_local_cnt = 0;
    }
}
static void key3_short_up_press(void)
{
    if(!dev_def.remote)
    {
      	if(RELAY3)
        {
            h595_val &= (~RELAY3_595);
        }
        else
        {
            if(dev_def.lock)
            {
                h595_val &= 0x0f;
            }
            h595_val |= RELAY3_595;
            
        }
        h595_val |= KEY3_595;
        SendTo595(h595_val);
        if(dev_def.setting == true)
        {
            dev_def.setting = false;
            update_time = 10;
        }
        else
        {
            update_time = 5;
        }
        dev_def.dev_channel[0].update_flag = true;
        dev_def.dev_channel[1].update_flag = true;
        dev_def.dev_channel[2].update_flag = true;
        dev_def.dev_channel[3].update_flag = true;
        dev_def.update_local_cnt = 0;
    }
}
static void key4_short_up_press(void)
{
    if(!dev_def.remote)
    {
      	if(RELAY4)
        {
            h595_val &= (~RELAY4_595);
            
        }
        else
        {
            if(dev_def.lock)
            {
                h595_val &= 0x0f;
            }
            h595_val |= RELAY4_595;
        }
        h595_val |= KEY4_595;
        SendTo595(h595_val);
        if(dev_def.setting == true)
        {
            dev_def.setting = false;
            update_time = 10;
        }
        else
        {
            update_time = 5;
        }
        dev_def.dev_channel[0].update_flag = true;
        dev_def.dev_channel[1].update_flag = true;
        dev_def.dev_channel[2].update_flag = true;
        dev_def.dev_channel[3].update_flag = true;
        dev_def.update_local_cnt = 0;
    }
}
static void key5_long_press(void)
{
    if(dev_def.remote)
    {
        dev_def.remote = false;
    }
    else
    {
        dev_def.remote = true;
        dev_def.remote_channel = 0;
    }
}
static void key1_long_up_press(void)
{
    h595_val |= KEY1_595;
    update_time = 10;
    dev_def.setting = true;
    SendTo595(h595_val);
}
static void key2_long_up_press(void)
{
	h595_val |= KEY2_595;
	update_time = 10;
	dev_def.setting = true;
	SendTo595(h595_val);
}
static void key3_long_up_press(void)
{
	h595_val |= KEY3_595;
	update_time = 10;
	dev_def.setting = true;
	SendTo595(h595_val);
}
static void key4_long_up_press(void)
{
	h595_val |= KEY4_595;
	update_time = 10;
	dev_def.setting = true;
	SendTo595(h595_val);
}
static void key1_twice_press(void)
{
    if(dev_def.dev_channel[0].channel_mode == DEV_JOGGING)
    {
        dev_def.dev_channel[0].channel_mode = DEV_SELFLOCK;
    }
    else
    {
        dev_def.dev_channel[0].channel_mode = DEV_JOGGING;
    }
    dev_def.dev_channel[0].update_flag = true; 
    dev_def.update_local_cnt = 0;
    save_mode_flag = true;
}
static void key2_twice_press(void)
{
    if(dev_def.dev_channel[1].channel_mode == DEV_JOGGING)
    {
        dev_def.dev_channel[1].channel_mode = DEV_SELFLOCK;
    }
    else
    {
        dev_def.dev_channel[1].channel_mode = DEV_JOGGING;
    }
    dev_def.dev_channel[1].update_flag = true; 
    dev_def.update_local_cnt = 0;
    save_mode_flag = true;
}
static void key3_twice_press(void)
{
    if(dev_def.dev_channel[2].channel_mode == DEV_JOGGING)
    {
        dev_def.dev_channel[2].channel_mode = DEV_SELFLOCK;
    }
    else
    {
        dev_def.dev_channel[2].channel_mode = DEV_JOGGING;
    }
    dev_def.dev_channel[2].update_flag = true; 
    dev_def.update_local_cnt = 0;
    save_mode_flag = true;
}
static void key4_twice_press(void)
{
    if(dev_def.dev_channel[3].channel_mode == DEV_JOGGING)
    {
        dev_def.dev_channel[3].channel_mode = DEV_SELFLOCK;
    }
    else
    {
        dev_def.dev_channel[3].channel_mode = DEV_JOGGING;
    }
    dev_def.dev_channel[3].update_flag = true; 
    dev_def.update_local_cnt = 0;
    save_mode_flag = true;
}
static void key5_twice_press(void)
{
    if(dev_def.lock == false)
    {
        dev_def.lock = true;
        MODE_LED = 1;
    }
    else
    {
        dev_def.lock = false;
        MODE_LED = 0;
    }
    h595_val &= 0x0f;
    SendTo595(h595_val);
    dev_def.dev_channel[0].update_flag = true;
    dev_def.dev_channel[1].update_flag = true;
    dev_def.dev_channel[2].update_flag = true;
    dev_def.dev_channel[3].update_flag = true;
    dev_def.update_local_cnt = 0;
    save_mode_flag = true;
}
static void registerKeys(void)
{
    keyRegisterSingle(key1_is_press,key1_short_press,key1_short_up_press,NULL,key1_long_up_press,key1_twice_press);
    keyRegisterSingle(key2_is_press,key2_short_press,key2_short_up_press,NULL,key2_long_up_press,key2_twice_press);
    keyRegisterSingle(key3_is_press,key3_short_press,key3_short_up_press,NULL,key3_long_up_press,key3_twice_press);
    keyRegisterSingle(key4_is_press,key4_short_press,key4_short_up_press,NULL,key4_long_up_press,key4_twice_press);
    keyRegisterSingle(key5_is_press,NULL,key5_short_up_press,key5_long_press,NULL,key5_twice_press);
}
void keyInit(void)
{
	P1M1 |= 0x1f;//初始化P10 -> P14为输入
   	P1M2 &= 0Xe0;
   	registerKeys();
}
