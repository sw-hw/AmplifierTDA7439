#include "vu.h"

static __IO VU_mode_t VU_mode;
static __IO int16_t   Cur_Segment_Led[VU_N];
static __IO uint32_t  Peak_Time_Led[VU_N];
static __IO int16_t   Peak_Segment_Led[VU_N];

static void VU_RedrawScaleLed(uint8_t segment, uint8_t channel, uint8_t state);
static void VU_EraseScale(void);
static void VU_ApplyMode(void);

// ===

static void VU_RedrawScaleLed(uint8_t segment, uint8_t channel, uint8_t state)
{
  // segment 0..(VU_MAX_NUM_LED - 1)
  const uint16_t left_offset = segment * VU_DIST_LED;
  const uint16_t color       = state ? (segment > 16 ? ILI9488_RED : (segment > 14 ? ILI9488_ORANGE : ILI9488_GREEN)) : ILI9488_COLOR_BACKGROUND;
  const uint16_t top_offset  = channel ? VU_TOP_OFFSET_SECOND_LED : VU_TOP_OFFSET_FIRST_LED;
  ILI9488_Draw_Rectangle(VU_LEFT_OFFSET_LED + left_offset, top_offset, VU_SIZE_LED, VU_SIZE_LED, color);
}

static void VU_EraseScale(void)
{
  ILI9488_Draw_Rectangle(VU_X0_COORD_ERASE, VU_Y0_COORD_ERASE, VU_WIDTH_ERASE, VU_HEIGHT_ERASE, ILI9488_COLOR_BACKGROUND);
}

static void VU_ApplyMode(void)
{
  int16_t i_channel;
  // ---
  VU_EraseScale();
  // ---
  ILI9488_Draw_Text("L",   VU_LEFT_OFFSET_LED_LABEL,              VU_TOP_OFFSET_FIRST_LED + ((VU_SIZE_LED - 16) >> 1),  VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("dB",  VU_LEFT_OFFSET_LED_LABEL,              VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-45", VU_LEFT_OFFSET_LED + VU_DIST_LED * 1,  VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-39", VU_LEFT_OFFSET_LED + VU_DIST_LED * 3,  VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-33", VU_LEFT_OFFSET_LED + VU_DIST_LED * 5,  VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-27", VU_LEFT_OFFSET_LED + VU_DIST_LED * 7,  VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-21", VU_LEFT_OFFSET_LED + VU_DIST_LED * 9,  VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-15", VU_LEFT_OFFSET_LED + VU_DIST_LED * 11, VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-9",  VU_LEFT_OFFSET_LED + VU_DIST_LED * 13, VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-6",  VU_LEFT_OFFSET_LED + VU_DIST_LED * 14, VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("-3",  VU_LEFT_OFFSET_LED + VU_DIST_LED * 15, VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text(" 0",  VU_LEFT_OFFSET_LED + VU_DIST_LED * 16, VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("+3",  VU_LEFT_OFFSET_LED + VU_DIST_LED * 17, VU_TOP_OFFSET_MID_LED,                                VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  ILI9488_Draw_Text("R",   VU_LEFT_OFFSET_LED_LABEL,              VU_TOP_OFFSET_SECOND_LED + ((VU_SIZE_LED - 16) >> 1), VU_COLOR_LABELS, VU_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
  // ---
  for(i_channel = 0; i_channel < VU_N; i_channel++)
  {
    Cur_Segment_Led[i_channel]  = -1;
    Peak_Segment_Led[i_channel] = -1;
  }
}

// ===

void VU_Init(void)
{
  VU_mode = VU_MODE_COLUMN_ONLY; // Set default mode
  VU_ApplyMode();
}

void VU_DisplaySignal(int16_t DB_Signals[])
{
  static uint8_t Peak_Time[VU_N] = { 0, 0 };
  const uint32_t hal_time = HAL_GetTick();
  int16_t i_channel;
  // ---
  for(i_channel = 0; i_channel < VU_N; i_channel++)
  {
    int16_t segment = (DB_Signals[i_channel] + (45 + 3)) / 3;
    if(segment < 0)
      segment = 0;
    else if(segment > VU_MAX_NUM_LED)
      segment = VU_MAX_NUM_LED;
    // ---
    if(segment > Cur_Segment_Led[i_channel])
    {
      Cur_Segment_Led[i_channel]++;
      VU_RedrawScaleLed(Cur_Segment_Led[i_channel], i_channel, 1);
    }
    else if(segment < Cur_Segment_Led[i_channel])
    {
      switch(VU_mode)
      {
        case VU_MODE_COLUMN_AND_PEAK:
          if(Cur_Segment_Led[i_channel] == Peak_Segment_Led[i_channel])
            break;
        case VU_MODE_COLUMN_ONLY:
          VU_RedrawScaleLed(Cur_Segment_Led[i_channel], i_channel, 0);
          break;
        case VU_MODE_enumMAX:
          break;
      }
      Cur_Segment_Led[i_channel]--;
    }
    // ---
    switch(VU_mode)
    {
      case VU_MODE_COLUMN_ONLY:
        break;
      case VU_MODE_COLUMN_AND_PEAK:
        if(Cur_Segment_Led[i_channel] >= Peak_Segment_Led[i_channel])
        {
          Peak_Segment_Led[i_channel] = Cur_Segment_Led[i_channel];
          Peak_Time_Led[i_channel]    = hal_time;
	  Peak_Time[i_channel]        = 1;
        }
	else if((Peak_Time[i_channel] && hal_time - Peak_Time_Led[i_channel] > VU_PEAK_TIME)
	    || (!Peak_Time[i_channel] && hal_time - Peak_Time_Led[i_channel] > VU_STEP_TIME))
        {
          Peak_Time_Led[i_channel] = hal_time;
          Peak_Time[i_channel]     = 0;
          VU_RedrawScaleLed(Peak_Segment_Led[i_channel], i_channel, 0);
          Peak_Segment_Led[i_channel]--;
          if(Cur_Segment_Led[i_channel] != Peak_Segment_Led[i_channel])
            VU_RedrawScaleLed(Peak_Segment_Led[i_channel], i_channel, 1);
        }
        break;
      case VU_MODE_enumMAX:
        break;
    }
  }
}

VU_mode_t VU_GetMode(void)
{
  return VU_mode;
}

void VU_NextMode(void)
{
  if(VU_mode == (VU_MODE_enumMAX - 1))
    return;
  VU_mode++;
  VU_ApplyMode();
}

void VU_PrevMode(void)
{
  if(VU_mode == 0)
    return;
  VU_mode--;
  VU_ApplyMode();
}
