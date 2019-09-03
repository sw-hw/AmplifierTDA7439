#ifndef VU_VU_H_
#define VU_VU_H_

#include "../ILI9488/ILI9488_STM32_Driver.h"
#include "../ILI9488/ILI9488_GFX.h"

#define	VU_DIST_LED							24
#define	VU_LEFT_OFFSET_LED_LABEL			10
#define	VU_TOP_OFFSET_FIRST_LED				180
#define	VU_TOP_OFFSET_SECOND_LED			250
#define	VU_TOP_OFFSET_MID_LED				((VU_TOP_OFFSET_FIRST_LED + VU_TOP_OFFSET_SECOND_LED) >> 1)
#define	VU_LEFT_OFFSET_LED					30
#define	VU_SIZE_LED							20
#define	VU_MAX_NUM_LED						17
#define	VU_COLOR_LABELS						ILI9488_WHITE
#define	VU_FONT_SIZE						1

void	VU_Init(void);
void 	VU_DisplaySignal(int16_t left, int16_t right);	// left and right signal in dB

#endif /* VU_VU_H_ */
