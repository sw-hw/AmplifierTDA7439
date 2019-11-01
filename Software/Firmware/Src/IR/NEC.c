#include "NEC.h"

static t_NEC_State NEC_State   = NEC_State_Ready;
static int16_t	   NEC_Command = -1; // -1 - not pressed any key; 0x00..0xFF - code of pressed key
static uint32_t	   NEC_Time;
static uint32_t	   NEC_DATA_BUFFER;
static uint8_t	   NEC_Bit_Counter;

// === Interface functions ===

void NEC_Tick(void)
{
	if(NEC_State > NEC_State_Ready && NEC_TIMER->CNT - NEC_Time > NEC_TIMEOUT)
		NEC_State = NEC_State_Ready;
}

void NEC_SignalEdge(void)
{
	switch(NEC_State)
	{
		case NEC_State_Ready:
		{
			NEC_Time = NEC_TIMER->CNT;
			NEC_State = NEC_State_Start;
			break;
		}
		case NEC_State_Start:
		{
			const uint32_t dt = NEC_TIMER->CNT - NEC_Time;
			if(NEC_TIME_START_MIN < dt && dt < NEC_TIME_START_MAX)
			{
				NEC_Bit_Counter = 0;
				NEC_Time = NEC_TIMER->CNT;
				NEC_State = NEC_State_DataBit;
			}
			else
				NEC_State = NEC_State_Ready;
			break;
		}
		case NEC_State_DataBit:
		{
			const uint32_t dt = NEC_TIMER->CNT - NEC_Time;
			NEC_DATA_BUFFER <<= 1;
			if(dt > NEC_TIME_EDGE_LOGIC)
				NEC_DATA_BUFFER |= 0x01;
			NEC_Bit_Counter++;
			if(NEC_Bit_Counter == 32) // if was received last bit
			{
				const uint8_t address_p = NEC_DATA_BUFFER >> 24;
				const uint8_t address_n = NEC_DATA_BUFFER >> 16;
				const uint8_t command_p = NEC_DATA_BUFFER >> 8;
				const uint8_t command_n = NEC_DATA_BUFFER;
				if(NEC_ADDRESS == address_p && address_p == ~address_n && command_p == ~command_n)
				{
					NEC_Command = command_p;
					NEC_Time = NEC_TIMER->CNT;
					NEC_State = NEC_State_WaitingRepeat;
				}
				else
					NEC_State = NEC_State_Ready;
			}
			else
				NEC_Time = NEC_TIMER->CNT;
			break;
		}
		case NEC_State_WaitingRepeat:
		{
			NEC_Time = NEC_TIMER->CNT;
			NEC_State = NEC_State_StartRepeat;
			break;
		}
		case NEC_State_StartRepeat:
		{
			const uint32_t dt = NEC_TIMER->CNT - NEC_Time;
			if(NEC_TIME_REPEAT_MIN < dt && dt < NEC_TIME_REPEAT_MAX)
			{
				NEC_Command = (NEC_DATA_BUFFER >> 8) & 0xFF;
				NEC_Time = NEC_TIMER->CNT;
				NEC_State = NEC_State_WaitingRepeat;
			}
			else
				NEC_State = NEC_State_Ready;
			break;
		}
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
