#ifndef __LED_H__
#define __LED_H__

#include "N76E003.h"

sbit MODE_LED = P0^6;
sbit RF_LED = P0^7;

void ledInit(void);

#endif
