#ifndef __KEY_DRIVER_H__
#define __KEY_DRIVER_H__

#include <stdio.h>
#include "N76E003.h"
#include "typedef.h"

#define  KEY_MAX_NUM      (5)
#define  KEY_SCAN_TIMES  (2)
#define  KEY_UP_TIMES     (2)
#define  KEY_LONG_TIMES   (300)
#define  KEY_TWICE_TIMES  (50)

typedef void (*KeyPressProcCb)(void);
typedef bool (*KeyIsPressCb)(void);

bool keyRegisterSingle(
                        KeyIsPressCb   _key_is_press,
                        KeyPressProcCb _key_short_press_proc_cb,
                        KeyPressProcCb _key_short_up_press_proc_cb,
                        KeyPressProcCb _key_long_press_proc_cb,
                        KeyPressProcCb _key_long_up_press_proc_cb,
                        KeyPressProcCb _key_press_twice_proc_cb);

void keyScan(void);

#endif