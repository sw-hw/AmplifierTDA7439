#ifndef VU_VU_H_
#define VU_VU_H_

#include "../ILI9488/ILI9488_STM32_Driver.h"
#include "../ILI9488/ILI9488_GFX.h"

#define	VU_N                     2
// ===
#define	VU_DIST_LED              24
#define	VU_LEFT_OFFSET_LED_LABEL 10
#define	VU_TOP_OFFSET_FIRST_LED	 180
#define	VU_TOP_OFFSET_SECOND_LED 250
#define	VU_TOP_OFFSET_MID_LED    ((VU_TOP_OFFSET_FIRST_LED + VU_TOP_OFFSET_SECOND_LED) >> 1)
#define	VU_LEFT_OFFSET_LED       30
#define	VU_SIZE_LED              20
#define	VU_MAX_NUM_LED           17
#define	VU_COLOR_LABELS          ILI9488_WHITE
#define	VU_FONT_SIZE             1
#define	VU_X0_COORD_ERASE        0
#define	VU_Y0_COORD_ERASE        (ILI9488_SCREEN_HEIGHT >> 1)
#define	VU_WIDTH_ERASE           ILI9488_SCREEN_WIDTH
#define	VU_HEIGHT_ERASE          ILI9488_SCREEN_HEIGHT
#define VU_PEAK_TIME             1000UL
#define VU_STEP_TIME             100UL

typedef enum {
  VU_MODE_COLUMN_ONLY = 0,
  VU_MODE_COLUMN_AND_PEAK,
  VU_MODE_enumMAX
} VU_mode_t;

void      VU_Init(void);
void      VU_DisplaySignal(int16_t DB_Signals[]); // left and right signals in dB
VU_mode_t VU_GetMode(void);
void      VU_NextMode(void);
void      VU_PrevMode(void);

#endif /* VU_VU_H_ */
