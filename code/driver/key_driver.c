#include "key_driver.h"
#include "typedef.h"


#define NO_KEY  (0)

typedef struct
{
	KeyIsPressCb   _key_is_press_cb;
    KeyPressProcCb _key_short_press_proc_cb;
    KeyPressProcCb _key_short_up_press_proc_cb;
    KeyPressProcCb _key_long_press_proc_cb;
    KeyPressProcCb _key_long_up_press_proc_cb;
    KeyPressProcCb _key_press_twice_proc_cb;
    uint8_t _key_val;
} KeySingle;

typedef struct
{
    KeySingle _key_single[KEY_MAX_NUM];
    uint8_t num;
}KeysDev;

typedef struct
{
    uint16_t _key_cnt;
    uint16_t _key_up;
    uint8_t _key_val;
    uint8_t twice_press_cnt;
    uint8_t twice_press_up_cnt;
    bool twice_press_flag;
    bool twice_press_up_flag;
    bool twice_flag;
}KEY_CTL;

typedef struct
{
    KEY_CTL key_ctl[KEY_MAX_NUM];
}KeysCtl;

KeysDev xdata keys_dev = {0};
KeysCtl xdata keys_ctl = {0};

static int8_t keyGet(void)
{
    uint8_t key_val = NO_KEY;
    uint8_t i = 0;
    for(i = 0; i < KEY_MAX_NUM; i++)
    {
        if(keys_dev._key_single[i]._key_is_press_cb)
        {
            if(true == keys_dev._key_single[i]._key_is_press_cb())
            {
                key_val |= keys_dev._key_single[i]._key_val;
            }
        }
    }
    return key_val;
}

bool keyRegisterSingle(KeyIsPressCb _key_is_press,
                            KeyPressProcCb _key_short_press_proc_cb,
                            KeyPressProcCb _key_short_up_press_proc_cb,
                            KeyPressProcCb _key_long_press_proc_cb,
                            KeyPressProcCb _key_long_up_press_proc_cb,
                            KeyPressProcCb _key_press_twice_proc_cb)
{
    uint8_t i = 1;
    if(keys_dev.num >= KEY_MAX_NUM)
    {
        return false;
    }
	keys_dev._key_single[keys_dev.num]._key_is_press_cb = _key_is_press;
	keys_dev._key_single[keys_dev.num]._key_short_press_proc_cb = _key_short_press_proc_cb;
	keys_dev._key_single[keys_dev.num]._key_short_up_press_proc_cb = _key_short_up_press_proc_cb;
	keys_dev._key_single[keys_dev.num]._key_long_press_proc_cb = _key_long_press_proc_cb;
	keys_dev._key_single[keys_dev.num]._key_long_up_press_proc_cb = _key_long_up_press_proc_cb;
	keys_dev._key_single[keys_dev.num]._key_press_twice_proc_cb = _key_press_twice_proc_cb;
	keys_dev._key_single[keys_dev.num]._key_val = i << keys_dev.num;
	keys_dev.num++;
	return true;
}

static void keyDeal(uint8_t key_val,uint8_t key_id)
{
    keys_ctl.key_ctl[key_id].twice_press_cnt++;
    keys_ctl.key_ctl[key_id].twice_press_up_cnt++;
    if(key_val == NO_KEY || key_val != keys_ctl.key_ctl[key_id]._key_val)
    {
        if(keys_ctl.key_ctl[key_id]._key_up < KEY_UP_TIMES)
        {
            keys_ctl.key_ctl[key_id]._key_up++;
        }
        else
        {
            if(keys_ctl.key_ctl[key_id]._key_cnt >= KEY_LONG_TIMES)
            {
                keys_ctl.key_ctl[key_id].twice_flag = false;
                if(keys_dev._key_single[key_id]._key_long_up_press_proc_cb)
                {
                    keys_dev._key_single[key_id]._key_long_up_press_proc_cb();
                }
            }
            else
            {
                if(keys_ctl.key_ctl[key_id]._key_cnt >= KEY_SCAN_TIMES)
                {
                    if(keys_dev._key_single[key_id]._key_press_twice_proc_cb)
                    {
                        if(keys_ctl.key_ctl[key_id].twice_flag == true)
                        {
                            keys_ctl.key_ctl[key_id].twice_flag = false;
                        }
                        else
                        {
                            if(keys_ctl.key_ctl[key_id].twice_press_up_flag == true)
                            {
                                /*处理短按抬键*/
                                if(keys_dev._key_single[key_id]._key_short_up_press_proc_cb)
                                {
                                    keys_dev._key_single[key_id]._key_short_up_press_proc_cb();
                                }
                            }
                            keys_ctl.key_ctl[key_id].twice_press_up_cnt = 0;
                            keys_ctl.key_ctl[key_id].twice_press_up_flag = true;
                        }
                    }
                    else
                    {
                        if(keys_dev._key_single[key_id]._key_short_up_press_proc_cb)
                        {
                            keys_dev._key_single[key_id]._key_short_up_press_proc_cb();
                        }
                    }
                }
            }
            keys_ctl.key_ctl[key_id]._key_val = key_val;
            keys_ctl.key_ctl[key_id]._key_cnt = 0;
        }
    }
    else
    {
        keys_ctl.key_ctl[key_id]._key_cnt++;
        if(keys_ctl.key_ctl[key_id]._key_cnt == KEY_SCAN_TIMES)
        {
            keys_ctl.key_ctl[key_id]._key_up = 0;
            if(keys_dev._key_single[key_id]._key_press_twice_proc_cb)
            {
                if(keys_ctl.key_ctl[key_id].twice_press_up_flag == true 
                    && keys_ctl.key_ctl[key_id].twice_press_cnt < KEY_TWICE_TIMES)
                {
                    keys_ctl.key_ctl[key_id].twice_flag = true;
                    keys_ctl.key_ctl[key_id].twice_press_up_flag = false;
                    keys_ctl.key_ctl[key_id].twice_press_flag = false;
                    /*双击相关处理*/
                    if(keys_dev._key_single[key_id]._key_press_twice_proc_cb)
                    {
                        keys_dev._key_single[key_id]._key_press_twice_proc_cb();
                    }
                }
                else
                {
                    keys_ctl.key_ctl[key_id].twice_flag = false;
                    keys_ctl.key_ctl[key_id].twice_press_flag = true;
                    keys_ctl.key_ctl[key_id].twice_press_cnt = 0;
                }
            }
            else
            {
                if(keys_dev._key_single[key_id]._key_short_press_proc_cb)
                {
                    keys_dev._key_single[key_id]._key_short_press_proc_cb(); 
                }
            }
        }
        else if (keys_ctl.key_ctl[key_id]._key_cnt == KEY_LONG_TIMES)
        {
            keys_ctl.key_ctl[key_id].twice_flag = false;
            if(keys_dev._key_single[key_id]._key_long_press_proc_cb)
            {
                keys_dev._key_single[key_id]._key_long_press_proc_cb();
            }
        }
    }
    if(keys_dev._key_single[key_id]._key_press_twice_proc_cb)
    {
        if(keys_ctl.key_ctl[key_id].twice_press_flag == true
            && keys_ctl.key_ctl[key_id].twice_press_cnt >= KEY_TWICE_TIMES)
        {
            keys_ctl.key_ctl[key_id].twice_press_flag = false;
            if(keys_dev._key_single[key_id]._key_short_press_proc_cb)
            {
                keys_dev._key_single[key_id]._key_short_press_proc_cb(); 
            }
        }
        if(keys_ctl.key_ctl[key_id].twice_press_up_flag == true
            && keys_ctl.key_ctl[key_id].twice_press_up_cnt >= KEY_TWICE_TIMES)
        {
            keys_ctl.key_ctl[key_id].twice_press_up_flag = false;
            if(keys_dev._key_single[key_id]._key_short_up_press_proc_cb)
            {
                keys_dev._key_single[key_id]._key_short_up_press_proc_cb();
            }
        }
    }
}
static void keysDeal(uint8_t key_val)
{
    uint8_t i = 0;
    uint8_t timer_cnt = 1;
    uint8_t key_temp = 0;
    for(i = 0; i < KEY_MAX_NUM; i++)
    {
        key_temp = key_val & (timer_cnt << i);
        keyDeal(key_temp,i);
    }
}
void keyScan(void)
{
    uint8_t key_val = NO_KEY;
    key_val = keyGet();
    keysDeal(key_val);
}
