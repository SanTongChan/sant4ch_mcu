#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "N76E003.h"
#include "typedef.h"

sbit RELAY1 = P0^1;
sbit RELAY2 = P0^0;
sbit RELAY3 = P0^4;
sbit RELAY4 = P0^3;
typedef enum
{
    DEV_SELFLOCK = 0,
    DEV_JOGGING = 1,
}ChannelMode;
typedef struct
{
    uint16_t timer_cnt;
    uint8_t update_flag;
    bool get_status;
    ChannelMode channel_mode;
}DevChannelDef;
typedef struct
{
    DevChannelDef dev_channel[4];
    uint8_t update_local_cnt;
    bool lock;
    bool setting;
}DevDef;
extern xdata DevDef dev_def;
extern uint8_t h595_val;
extern bool key_scan_flag;
extern bool update_local_flag;
extern bool syn_app_flag;
extern bool deal_jogging;
extern bool deal_lock;
extern uint8_t update_time;
extern bool save_mode_flag;
extern bool deal_remote_flag;
void logicInit(void);
void SendTo595(uint8_t val);
void dealLogic(void);
void saveModeToFlash(void);
void dealRemote(void);
#endif
