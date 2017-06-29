#include <intrins.h>
#include "logic.h"

sbit RCK = P1^7;
sbit SCK = P1^6;
sbit SER = P3^0;

void logicInit(void)
{
    //初始化P16 P17 P30为推挽输出
    P1M1 &= 0x3f;
   	P1M2 |= 0Xc0;
   	P3M1 &= 0xfe;
    P3M2 |= 0x01;
    SER = 1;
}
void SendTo595(uint8_t val)
{
	char i=0;
	uint8_t temp = val;
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
}


