#include "led.h"

void ledInit(void)
{
    P0M1 &= 0x3f;
   	P0M2 |= 0XC0; 
   	RF_LED = 0;
}



