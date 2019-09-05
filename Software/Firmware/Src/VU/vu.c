#include "vu.h"

static __IO VU_mode_t VU_mode;

static __IO int16_t left_cur_segment_led;
static __IO int16_t right_cur_segment_led;

static __IO uint32_t left_peak_time_led;
static __IO uint32_t right_peak_time_led;
static __IO int16_t	left_peak_segment_led;
static __IO int16_t	right_peak_segment_led;

static void	VU_RedrawScaleLed(uint8_t segment, uint8_t channel, uint8_t state);
static void	VU_EraseScale(void);
static void	VU_ApplyMode();

// ===

static void	VU_RedrawScaleLed(uint8_t segment, uint8_t channel, uint8_t state)
{
	// segment 0..(VU_MAX_NUM_LED - 1)
	const uint16_t left_offset = segment * VU_DIST_LED;
	const uint16_t color	   = state ? (segment > 15 ? ILI9488_RED : (segment > 13 ? ILI9488_ORANGE : ILI9488_GREEN)) : ILI9488_COLOR_BACKGROUND;
	const uint16_t top_offset  = channel ? VU_TOP_OFFSET_SECOND_LED : VU_TOP_OFFSET_FIRST_LED;
	ILI9488_Draw_Filled_Rectangle_Coord(VU_LEFT_OFFSET_LED + left_offset, top_offset,  VU_LEFT_OFFSET_LED + left_offset + VU_SIZE_LED,
			top_offset + VU_SIZE_LED, color);
}

static void	VU_EraseScale(void)
{
	ILI9488_Draw_Filled_Rectangle_Coord(VU_X0_COORD_ERASE, VU_Y0_COORD_ERASE, VU_X1_COORD_ERASE, VU_Y1_COORD_ERASE, ILI9488_COLOR_BACKGROUND);
}

static void	VU_ApplyMode()
{
	VU_EraseScale();
	// ---
	ILI9488_Draw_Text("L",	  		  VU_LEFT_OFFSET_LED_LABEL,   				  VU_TOP_OFFSET_FIRST_LED + ((VU_SIZE_LED - 16) >> 1),
			VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("dB",	  	  	  VU_LEFT_OFFSET_LED_LABEL,  				  VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-45",	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 1,  VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-39", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 3,  VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-33", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 5,  VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-27", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 7,  VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-21", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 9,  VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-15", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 11, VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-9", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 13, VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-6", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 14, VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("-3", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 15, VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text(" 0", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 16, VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("+3", 	  	  VU_LEFT_OFFSET_LED + VU_DIST_LED * 17, VU_TOP_OFFSET_MID_LED, VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("R",	  		  VU_LEFT_OFFSET_LED_LABEL,					  VU_TOP_OFFSET_SECOND_LED + ((VU_SIZE_LED - 16) >> 1),
			VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	// ---
	left_cur_segment_led  = -1;
	right_cur_segment_led = -1;
	left_peak_time_led = ~ 0UL;
	right_peak_time_led = ~ 0UL;
	left_peak_segment_led = -1;
	right_peak_segment_led = -1;
}

// ===

void	VU_Init(void)
{
	VU_mode = VU_MODE_COLUMN_ONLY; // Set default mode
	VU_ApplyMode();
}

void	VU_DisplaySignal(int16_t left, int16_t right)
{
	int16_t left_segment = (left + (45 + 3)) / 3;
	int16_t right_segment = (right + (45 + 3)) / 3;
	// ---
	if(left_segment < 0)
		left_segment = 0;
	else if(left_segment > VU_MAX_NUM_LED)
		left_segment = VU_MAX_NUM_LED;
	// ---
	if(left_segment > left_cur_segment_led)
	{
		left_cur_segment_led++;
		VU_RedrawScaleLed(left_cur_segment_led, 0, 1);
	}
	else if(left_segment < left_cur_segment_led)
	{
		switch(VU_mode)
		{
			case VU_MODE_COLUMN_AND_PEAK:
				if(left_cur_segment_led == left_peak_segment_led)
					break;
			case VU_MODE_COLUMN_ONLY:
				VU_RedrawScaleLed(left_cur_segment_led, 0, 0);
				break;
			case VU_MODE_enumMAX:
				break;
		}
		left_cur_segment_led--;
	}
	// ---
	if(right_segment < 0)
		right_segment = 0;
	else if(right_segment > VU_MAX_NUM_LED)
		right_segment = VU_MAX_NUM_LED;
	// ---
	if(right_segment > right_cur_segment_led)
	{
		right_cur_segment_led++;
		VU_RedrawScaleLed(right_cur_segment_led, 1, 1);
	}
	else if(right_segment < right_cur_segment_led)
	{
		switch(VU_mode)
		{
			case VU_MODE_COLUMN_AND_PEAK:
				if(right_cur_segment_led == right_peak_segment_led)
					break;
			case VU_MODE_COLUMN_ONLY:
				VU_RedrawScaleLed(right_cur_segment_led, 1, 0);
				break;
			case VU_MODE_enumMAX:
				break;
		}
		right_cur_segment_led--;
	}
	// ---
	switch(VU_mode)
	{
		case VU_MODE_COLUMN_ONLY:
			break;
		case VU_MODE_COLUMN_AND_PEAK:
			if(left_cur_segment_led >= left_peak_segment_led)
			{
				left_peak_segment_led = left_cur_segment_led;
				left_peak_time_led = HAL_GetTick() + VU_PEAK_TIME;
			}
			else if(HAL_GetTick() > left_peak_time_led)
			{
				left_peak_time_led = HAL_GetTick() + VU_STEP_TIME;
				VU_RedrawScaleLed(left_peak_segment_led, 0, 0);
				left_peak_segment_led--;
				if(left_cur_segment_led != left_peak_segment_led)
					VU_RedrawScaleLed(left_peak_segment_led, 0, 1);
			}
			// ---
			if(right_cur_segment_led >= right_peak_segment_led)
			{
				right_peak_segment_led = right_cur_segment_led;
				right_peak_time_led = HAL_GetTick() + VU_PEAK_TIME;
			}
			else if(HAL_GetTick() > right_peak_time_led)
			{
				right_peak_time_led = HAL_GetTick() + VU_STEP_TIME;
				VU_RedrawScaleLed(right_peak_segment_led, 1, 0);
				right_peak_segment_led--;
				if(right_cur_segment_led != right_peak_segment_led)
					VU_RedrawScaleLed(right_peak_segment_led, 1, 1);
			}
			break;
		case VU_MODE_enumMAX:
			break;
	}
}

VU_mode_t	VU_GetMode(void)
{
	return VU_mode;
}

void		VU_NextMode(void)
{
	if(VU_mode == (VU_MODE_enumMAX - 1))
		VU_mode = 0;
	else
		VU_mode++;
	VU_ApplyMode();
}

void		VU_PrevMode(void)
{
	if(VU_mode == 0)
		VU_mode = (VU_MODE_enumMAX - 1);
	else
		VU_mode--;
	VU_ApplyMode();
}
