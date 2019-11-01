#ifndef IR_NEC_H_
#define IR_NEC_H_

#include "tim.h"

// === configuration ===
#define NEC_TIMER			TIM2
#define NEC_ADDRESS			0x00
// ---
#define NEC_TIME_START_MIN  8000UL 		// us
#define NEC_TIME_START_MAX	10000UL 	// us
#define	NEC_TIME_PAUSE_MIN	4000UL		// us
#define	NEC_TIME_PAUSE_MAX	5000UL		// us
#define	NEC_TIME_LOG_0_MAX	1688UL		// us
#define	NEC_TIME_LOG_1_MAX	3375UL		// us
#define	NEC_TIMEOUT			200000UL	// us

// === Interface functions ===
void NEC_Tick(void);
void NEC_SetEdge(uint8_t rising);
int16_t NEC_GetCommand(void);

#endif
