#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "N76E003.h"
#include "typedef.h"

sbit RELAY1 = P0^1;
sbit RELAY2 = P0^0;
sbit RELAY3 = P0^3;
sbit RELAY4 = P0^2;

extern uint8_t h595_val;
extern bool key_scan_flag;
extern bool update_status_flag;
extern bool led_blink_flag;
void logicInit(void);
void SendTo595(uint8_t val);
void dealLogic(void);

#endif
