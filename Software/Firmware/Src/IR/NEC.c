#include "NEC.h"

typedef enum {
	NEC_State_Ready = 0,
	NEC_State_Start,     // 9ms
	NEC_State_Pause,	 // 4.5ms
	NEC_State_DataBit	 // address p (8 bit), address n (8 bit), command p (8 bit), command n (8 bit)
	// TODO NEC_State_WaitingRepeat
} t_NEC_State;

static t_NEC_State NEC_State = NEC_State_Ready;
static uint32_t	   NEC_Time    = 0UL;
static uint32_t	   NEC_Data    = 0UL;
static uint8_t	   NEC_Index   = 0;
static int16_t	   NEC_Command = -1;

static int8_t newDataBit(uint32_t dt); // -1 - error; 0 - bit was added; 1 - last bit was added
static int8_t checkData(void);         // -1 - error checksum; 0 - not corrected address; 1 - OK

static int8_t newDataBit(uint32_t dt)
{
	if(NEC_Index > 31)
		return -1;
	if(dt < NEC_TIME_LOG_0_MAX)
		NEC_Data &= ~(1UL << NEC_Index);
	else if(dt < NEC_TIME_LOG_1_MAX)
		NEC_Data |= (1UL << NEC_Index);
	else
		return -1;
	NEC_Index++;
	if(NEC_Index < 32)
		return 0;
	else
		return 1;
}

static int8_t checkData(void)
{
	const uint8_t address_p = NEC_Data >> 24;
	const uint8_t address_n = NEC_Data >> 16;
	const uint8_t command_p = NEC_Data >> 8;
	const uint8_t command_n = NEC_Data;
	if(address_p == ~address_n && command_p == ~command_n)
	{
		if(NEC_ADDRESS == address_p)
		{
			NEC_Command = command_p;
			return 1;
		}
		else
			return 0;
	}
	else
		return -1;
}

// === Interface functions ===

void NEC_Tick(void)
{
	if(NEC_State > NEC_State_Ready && NEC_TIMER->CNT - NEC_Time > NEC_TIMEOUT)
		NEC_State = NEC_State_Ready;
}

void NEC_SetEdge(uint8_t rising)
{
	switch(NEC_State)
	{
		case NEC_State_Ready:
			if(rising)
			{
				NEC_Time = NEC_TIMER->CNT;
				NEC_State = NEC_State_Start;
			}
			break;
		case NEC_State_Start:
			if(rising)
				NEC_State = NEC_State_Ready;
			else
			{
				const uint32_t dt = NEC_TIMER->CNT - NEC_Time;
				if(NEC_TIME_START_MIN < dt && dt < NEC_TIME_START_MAX)
				{
					NEC_Time = NEC_TIMER->CNT;
					NEC_State = NEC_State_Pause;
				}
				else
					NEC_State = NEC_State_Ready;
			}
			break;
		case NEC_State_Pause:
			if(rising)
			{
				const uint32_t dt = NEC_TIMER->CNT - NEC_Time;
				if(NEC_TIME_PAUSE_MIN < dt && dt < NEC_TIME_PAUSE_MAX)
				{
					NEC_Index = 0;
					NEC_Time = NEC_TIMER->CNT;
					NEC_State = NEC_State_DataBit;
				}
				else
					NEC_State = NEC_State_Ready;
			}
			else
				NEC_State = NEC_State_Ready;
			break;
		case NEC_State_DataBit:
			if(rising)
			{
				const int8_t res = newDataBit(NEC_TIMER->CNT - NEC_Time);
				switch(res)
				{
					case -1:
						NEC_State = NEC_State_Ready;
						break;
					case 0:
						NEC_Time = NEC_TIMER->CNT;
						break;
					case 1:
						checkData();
						NEC_State = NEC_State_Ready;
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
}

int16_t NEC_GetCommand(void)
{
	int16_t command = NEC_Command;
	NEC_Command = -1;
	return command;
}
