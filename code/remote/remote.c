#include "remote.h"
#include "N76E003.h"
#include "led.h"
#if 1
sbit RF_PIN = P1^5;

typedef struct
{
	uint8_t end_flag;
	uint8_t sync_flag;
	uint16_t sync_time;
	uint16_t rf_data[48];
}RecvData;
static RecvData xdata recv_data = {0};

IrData xdata ir_data = {0};

static void getCaptureVal(uint16_t capture_value)
{
	static uint8_t cnt = 0;
	if(!recv_data.sync_flag)
	{
		if(capture_value >= 250 && RF_PIN == 1)
		{
			recv_data.sync_time = capture_value;
			recv_data.sync_flag = true;
			
		}
	}
	else
	{
		if(recv_data.sync_time > (capture_value * 5))
		{
			recv_data.rf_data[cnt] = capture_value;
			cnt++;
		}
		else
		{
			recv_data.sync_flag = false;
			cnt = 0;
		}
		if(cnt >= 48)
		{
			cnt = 0;
			recv_data.end_flag = true;
			recv_data.sync_flag = false;
		}
	}

}

void clearIrData(void)
{
	ir_data.cnt = 0;
	ir_data.ir_data = 0;
	ir_data.sync_time = 0;
}
void analyzeRfData(void)
{
	uint8_t i = 0;
	uint32_t temp = 0;
	uint16_t unit_time = 0;
	uint16_t temp_time = 0;
	uint16_t threshold_time = 0;
	uint16_t sync_threshold = 0;
	uint16_t short_time = 0;
	uint16_t long_time = 0;
	if(recv_data.end_flag == true)
	{
		recv_data.end_flag = false;
		if(ir_data.cnt == 0)
		{
			ir_data.high_time = recv_data.rf_data[0];
			ir_data.low_time = recv_data.rf_data[1];
			ir_data.sync_time = recv_data.sync_time;
		}
		unit_time = ir_data.high_time + ir_data.low_time;
		threshold_time = unit_time / 5;
		sync_threshold = ir_data.sync_time / 5;
		for(i = 0; i < 48; i = i + 2)
		{
			temp_time = recv_data.rf_data[i] + recv_data.rf_data[i + 1];
			if((temp_time > unit_time + threshold_time)
				|| (temp_time < unit_time - threshold_time))
			{
				ir_data.timer_cnt++;
				return;
			}
			if(recv_data.rf_data[i] > recv_data.rf_data[i + 1])
			{
				temp = (temp << 1) | 0x01;
				short_time += recv_data.rf_data[i + 1];
				long_time += recv_data.rf_data[i];
			}
			else if(recv_data.rf_data[i] < recv_data.rf_data[i + 1])
			{
				temp = temp << 1;
				long_time += recv_data.rf_data[i + 1];
				short_time += recv_data.rf_data[i];
			}
			else
			{
				ir_data.timer_cnt++;
				return;
			}
		}
		if(!(ir_data.sync_time < threshold_time + recv_data.sync_time
			&& ir_data.sync_time > recv_data.sync_time - threshold_time
			&& temp == ir_data.ir_data))
		{
			ir_data.sync_time = recv_data.sync_time;
			ir_data.ir_data = temp;
			ir_data.cnt = 1;
			ir_data.timer_cnt = 0;
			ir_data.low_time = (uint16_t)((short_time / 24) + 0.5);
			ir_data.high_time = (uint16_t)((long_time / 24) + 0.5);
		}
		else
		{
			ir_data.cnt++;
			ir_data.timer_cnt = 0;
		}
	}
	else
	{
		ir_data.timer_cnt++;
	}
}
static void Timer2Init(void)
{
	T2CON&=~0x01;
	T2MOD &= 0X0F;
	T2MOD|=0xD0;
	T2MOD|=0X08;	

	CAPCON0|=0x10;
	CAPCON1|=0x03;
	CAPCON1&=0xfe;
	//CAPCON2|=0x10;
	CAPCON3&=0xF0;
	CAPCON3|=0x08;
}
static void captureIOInit(void)
{
	P1M1 &= 0xDF;
	P1M2 &= 0XDF;
}
void captureInit(void)
{
	captureIOInit();
	Timer2Init();
	EIE |= 0X04;
	TR2 = 1;
}
void Capture_ISR (void) interrupt 12
{
	uint16_t capture_val = 0;
	if(CAPCON0 & 0X01 )
	{
		CAPCON0 &= ~0x01;
		capture_val = C0H << 8 | C0L;	
		getCaptureVal(capture_val);	   
		
	}
}
#endif
