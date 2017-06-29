#include "key.h"
#include "key_driver.h"
#include "led.h"

sbit KEY1 = P1^3;
sbit KEY2 = P1^2;
sbit KEY3 = P1^1;
sbit KEY4 = P1^0;
sbit KEY5 = P1^4;
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
    MODE_LED = !MODE_LED;
}
static void key2_short_press(void)
{
    MODE_LED = !MODE_LED;
}
static void key3_short_press(void)
{
    MODE_LED = !MODE_LED;
}
static void key4_short_press(void)
{
    MODE_LED = !MODE_LED;
}
static void key5_short_press(void)
{
    MODE_LED = !MODE_LED;
}
static void registerKeys(void)
{
    keyRegisterSingle(key1_is_press,key1_short_press,NULL,NULL,NULL,NULL);
    keyRegisterSingle(key2_is_press,key2_short_press,NULL,NULL,NULL,NULL);
    keyRegisterSingle(key3_is_press,key3_short_press,NULL,NULL,NULL,NULL);
    keyRegisterSingle(key4_is_press,key4_short_press,NULL,NULL,NULL,NULL);
    keyRegisterSingle(key5_is_press,key5_short_press,NULL,NULL,NULL,NULL);
}
void keyInit(void)
{
	P1M1 |= 0x1f;//初始化P10 -> P14为输入
   	P1M2 &= 0Xe0;
   	registerKeys();
}
