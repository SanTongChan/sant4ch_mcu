#include <stdio.h>
#include "led.h"
#include "timer.h"
#include "key.h"
#include "logic.h"

void main(void)
{
	ledInit();   
	timerInit();
	keyInit();
	logicInit();
	SendTo595(0xff);
	MODE_LED = 1;
	RF_LED = 1;
	while(1);
}