#ifndef IR_NEC_H_
#define IR_NEC_H_

#include "tim.h"

// === configuration ===
#define NEC_TIMER				TIM2
#define NEC_ADDRESS				0x00	// TODO
// ---
#define NEC_TIME_START_MIN  	12000UL 	// us
#define NEC_TIME_START_MAX		15000UL 	// us
#define	NEC_TIME_REPEAT_MIN		10500UL		// us
#define	NEC_TIME_REPEAT_MAX		12000UL		// us
#define	NEC_TIME_EDGE_LOGIC		1688UL		// us
#define	NEC_TIMEOUT				200000UL	// us

// ======================
typedef enum {
	NEC_State_Ready = 0,
	NEC_State_Start,         // 9ms + 4.5ms
	NEC_State_DataBit,	     // address p (8 bit), address n (8 bit), command p (8 bit), command n (8 bit)
	NEC_State_WaitingRepeat,
	NEC_State_StartRepeat
} t_NEC_State;

// === Interface functions ===
void NEC_Tick(void);
void NEC_SignalEdge(void);
int16_t NEC_GetCommand(void);

#endif