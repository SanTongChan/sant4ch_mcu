#ifndef __REMOTE_H__
#define __REMOTE_H__

#include "typedef.h"

typedef struct
{
	uint16_t sync_time;
	uint16_t high_time;
	uint16_t low_time;
	uint32_t ir_data;
	uint8_t cnt;
	uint16_t timer_cnt;
}IrData;

extern  IrData ir_data;

void captureInit(void);
void analyzeRfData(void);

#endif
