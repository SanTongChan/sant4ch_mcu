#ifndef _KEY_H_
#define _KEY_H_

#include "N76E003.h"

#define KEY3_595 (0X01)
#define KEY2_595 (0X02)
#define KEY1_595 (0X04)
#define KEY4_595 (0X08)

#define RELAY4_595	 (0X10)
#define RELAY3_595   (0X20)
#define RELAY2_595   (0X40)
#define RELAY1_595   (0X80)


void keyInit(void);

#endif

