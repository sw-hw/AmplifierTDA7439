#include "vu.h"

static __IO VU_mode_t VU_mode;

static __IO uint16_t left_cur_segment_led = 0;
static __IO uint16_t right_cur_segment_led = 0;

static void	VU_RedrawScaleLed(uint8_t segment, uint8_t channel, uint8_t state);
static void	VU_EraseScale(void);

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

void	VU_Init(void)
{
	VU_SetMode(VU_MODE_LED); // Set default mode
}

void	VU_DisplaySignal(int16_t left, int16_t right)
{
	switch(VU_mode)
	{
		case VU_MODE_LED:
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
				VU_RedrawScaleLed(left_cur_segment_led, 0, 0);
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
				VU_RedrawScaleLed(right_cur_segment_led, 1, 0);
				right_cur_segment_led--;
			}
			break;
		}
		case VU_MODE_ARROW:
			// TODO
			break;
	}
}

void	VU_SetMode(VU_mode_t mode)
{
	VU_mode = mode;
	switch(VU_mode)
	{
		case VU_MODE_LED:
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
			left_cur_segment_led  = 0;
			right_cur_segment_led = 0;
			VU_RedrawScaleLed(0, 0, 1);
			VU_RedrawScaleLed(0, 1, 1);
			break;
		case VU_MODE_ARROW:
			VU_EraseScale();
			// ---
			// TODO
			break;
	}
}

VU_mode_t	VU_GetMode(void)
{
	return VU_mode;
}
