#include "TDA7439.h"
#include <stdio.h>

static uint8_t TDA7439_data[9];
static t_TDA7439_Marker TDA7439_marker;

static void TDA7439_I2C_Transmit(void);
static void TDA7439_TurnOn(void);
static void TDA7439_TurnOff(void);
static void TDA7439_DisplayInit(void);
static void TDA7439_DisplayRedrawSelector(void);
static void TDA7439_DisplayRedrawVal(uint8_t draw_all); // 1 - draw all values; 0 - draw value according TDA7439_marker
static void TDA7439_DisplayRedrawScale(uint8_t segment, uint8_t channel, uint8_t state);

static void TDA7439_I2C_Transmit(void)
{
	HAL_I2C_Master_Transmit(TDA7439_HI2C_INSTANCE, TDA7439_I2C_ADDRESS, TDA7439_data, sizeof(TDA7439_data), TDA7439_I2C_TIMEOUT);
}

static void TDA7439_TurnOn(void)
{
	TDA7439_marker = TDA7439_MARKER_HEAD;
	HAL_GPIO_WritePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	TDA7439_data[0] = 0x10; 		// start sub-address and auto increment mode
	TDA7439_data[1] = 0b11; 		// multiplexer
	TDA7439_data[2] = 0b0000;		// input gain
	TDA7439_data[3] = 0b0000; 		// volume
	TDA7439_data[4] = 0b1101; 		// bass (0b0000 to 0b0111 to 0b1000)
	TDA7439_data[5] = 0b0000; 		// mid-range (0b0000 to 0b0111 to 0b1000)
	TDA7439_data[6] = 0b1000; 		// treble (0b0000 to 0b01111 to 0b1000)
	TDA7439_data[7] = 30;			// first speaker attenuation
	TDA7439_data[8] = 30;			// second speaker attenuation
	TDA7439_I2C_Transmit();
	// ---
	TDA7439_DisplayInit();
	TDA7439_DisplayRedrawVal(1);
	TDA7439_DisplayRedrawSelector();
}

static void TDA7439_TurnOff(void)
{
	ILI9488_LedDisable();
	// TODO turn OFF mode ILI9488
	HAL_GPIO_WritePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}

static void TDA7439_DisplayInit(void)
{
	ILI9488_Init(); //initial driver setup to drive ili9488
	ILI9488_Fill_Screen(TDA7439_COLOR_BACKGROUND);
	ILI9488_LedEnable();
	// ===
	ILI9488_Draw_Text("Headphone:",	  TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Power amp:",	  TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Multiplexer:", TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Input gain:",  TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Volume:",	  TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	// ---
	ILI9488_Draw_Text("Bass:",		  TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Mid:",		  TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Treble:",	  TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Balance:",	  TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
	// ---
	// TODO scale init (call TDA7439_DisplayRedrawScale in loop with params for set all segments in 0 state)
}

static void TDA7439_DisplayRedrawSelector(void)
{
	switch(TDA7439_marker)
	{
		case TDA7439_MARKER_HEAD:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_POW_AMP:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_MULTIPLEXER:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_GAIN:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_VOLUME:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_BASS:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FIRST_ROW, 	TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_MID:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FIRST_ROW, 	TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_TREBLE:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_BALANCE:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_enumMAX:
			break;
	}
}

static void TDA7439_DisplayRedrawVal(uint8_t draw_all)
{
	char str[10];
	int8_t val;
	t_TDA7439_Marker marker;
	if(draw_all)
		marker = 0;
	else
		marker = TDA7439_marker;
	do {
		switch(marker)
		{
			case TDA7439_MARKER_HEAD:
				if(HAL_GPIO_ReadPin(POW_HEAD_GPIO_Port, POW_HEAD_Pin))
					ILI9488_Draw_Text("ON ", TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_FIRST_ROW,  ILI9488_GREEN, 	 TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				else
					ILI9488_Draw_Text("OFF", TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_POW_AMP:
				// TODO
				ILI9488_Draw_Text("OFF",  TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_MULTIPLEXER:
				sprintf(str, "in %d", 4 - TDA7439_data[1]);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_GAIN:
				sprintf(str, "%d dB  ", (TDA7439_data[2] << 1) - (int8_t)TDA7439_data[3]);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
				// ---
			case TDA7439_MARKER_VOLUME:
				sprintf(str, "%d dB  ", -(int8_t)(TDA7439_data[7] < TDA7439_data[8] ? TDA7439_data[7] : TDA7439_data[8]));
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_FIRST_VALS, TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_BASS:
				TDA7439_EQ_TO_DB(TDA7439_data[4], val);
				sprintf(str, "%d dB  ", val);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_FIRST_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_MID:
				TDA7439_EQ_TO_DB(TDA7439_data[5], val);
				sprintf(str, "%d dB  ", val);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_SECOND_ROW,  TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_TREBLE:
				TDA7439_EQ_TO_DB(TDA7439_data[6], val);
				sprintf(str, "%d dB  ", val);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_THIRD_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_BALANCE:
				val = (int8_t)TDA7439_data[7] - (int8_t)TDA7439_data[8];
				if(val < 0)
				{
					sprintf(str, "L %d dB ", val);
					ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				}
				else if(val > 0)
				{
					sprintf(str, "R %d dB ", -val);
					ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				}
				else
					ILI9488_Draw_Text("equal   ", TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_FOURTH_ROW, ILI9488_GREEN, TDA7439_FONT_SIZE, TDA7439_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_enumMAX:
				break;
		}
		marker++;
	} while(draw_all && marker < TDA7439_MARKER_enumMAX);
}

static void TDA7439_DisplayRedrawScale(uint8_t segment, uint8_t channel, uint8_t state)
{
	// TODO
}

// === Interface functions ===

void TDA7439_EncoderButton(uint8_t state)
{
	static uint32_t last_time = ~ 0UL;
	if(HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin))
	{
		static uint8_t short_push = 0;
		if(state)
		{
			last_time = HAL_GetTick();
			if(short_push)
			{
				short_push = 0;
				if(TDA7439_marker == (TDA7439_MARKER_enumMAX - 1))
					TDA7439_marker = 0;
				else
					TDA7439_marker++;
				TDA7439_DisplayRedrawSelector();
			}
		}
		else if(last_time != (~ 0UL))
		{
			if(HAL_GetTick() - last_time > TDA7439_TIME_TURN_OFF)
			{
				TDA7439_TurnOff();
				last_time = ~ 0UL;
				short_push = 0;
			}
			else
				short_push = 1;
		}
	}
	else
	{
		if(state)
			last_time = HAL_GetTick();
		else if(last_time != (~ 0UL) && HAL_GetTick() - last_time > TDA7439_TIME_TURN_ON)
		{
			TDA7439_TurnOn();
			last_time = ~ 0UL;
		}
	}
}

void TDA7439_EncoderRotate(EncoderRotate_t rotate)
{
	if(!HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin))
		return;
	switch(TDA7439_marker)
	{
		case TDA7439_MARKER_HEAD:
			if(rotate == ENCODER_ROTATE_R)
				HAL_GPIO_WritePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin, GPIO_PIN_SET);
			else if(rotate == ENCODER_ROTATE_L)
				HAL_GPIO_WritePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin, GPIO_PIN_RESET);
			break;
		case TDA7439_MARKER_POW_AMP:
			// TODO
			break;
		case TDA7439_MARKER_MULTIPLEXER:
			if(rotate == ENCODER_ROTATE_R)
			{
				if(TDA7439_data[1] == 0)
					TDA7439_data[1] = 0x03;
				else
					TDA7439_data[1]--;
			}
			else if(rotate == ENCODER_ROTATE_L)
			{
				if(TDA7439_data[1] == 0x03)
					TDA7439_data[1] = 0;
				else
					TDA7439_data[1]++;
			}
			break;
		case TDA7439_MARKER_GAIN:
			if(rotate == ENCODER_ROTATE_R)
			{
				if(TDA7439_data[3] == 0) // if volume equals 0 dB
				{
					if(TDA7439_data[2] != 0x0F) // if gain not equals +30 dB
						TDA7439_data[2]++;
				}
				else
					TDA7439_data[3]--;
			}
			else if(rotate == ENCODER_ROTATE_L)
			{
				if(TDA7439_data[2] == 0) // if gain equals 0 dB
				{
					if(TDA7439_data[3] != 40) // if volume not equals -40 dB
						TDA7439_data[3]++;
				}
				else
					TDA7439_data[2]--;
			}
			break;
		case TDA7439_MARKER_VOLUME:
			if(rotate == ENCODER_ROTATE_R)
			{
				if(TDA7439_data[7] != 0 && TDA7439_data[8] != 0) // if both speaker attenuates are not equals 0 dB
				{
					TDA7439_data[7]--;
					TDA7439_data[8]--;
				}
			}
			else if(rotate == ENCODER_ROTATE_L)
			{
				if(TDA7439_data[7] != 72 && TDA7439_data[8] != 72) // if both speaker attenuates are not equals -72 dB
				{
					TDA7439_data[7]++;
					TDA7439_data[8]++;
				}
			}
			break;
		case TDA7439_MARKER_BASS:
			if(rotate == ENCODER_ROTATE_R)
				TDA7439_EQ_INC(TDA7439_data[4])
			else if(rotate == ENCODER_ROTATE_L)
				TDA7439_EQ_DEC(TDA7439_data[4])
			break;
		case TDA7439_MARKER_MID:
			if(rotate == ENCODER_ROTATE_R)
				TDA7439_EQ_INC(TDA7439_data[5])
			else if(rotate == ENCODER_ROTATE_L)
				TDA7439_EQ_DEC(TDA7439_data[5])
			break;
		case TDA7439_MARKER_TREBLE:
			if(rotate == ENCODER_ROTATE_R)
				TDA7439_EQ_INC(TDA7439_data[6])
			else if(rotate == ENCODER_ROTATE_L)
				TDA7439_EQ_DEC(TDA7439_data[6])
			break;
		case TDA7439_MARKER_BALANCE:
			if(rotate == ENCODER_ROTATE_R)
			{
				if(TDA7439_data[8] > TDA7439_data[7])
					TDA7439_data[8]--;
				else if(TDA7439_data[7] != 72) // if first speaker is not equals -72 dB
					TDA7439_data[7]++;
			}
			else if(rotate == ENCODER_ROTATE_L)
			{
				if(TDA7439_data[7] > TDA7439_data[8])
					TDA7439_data[7]--;
				else if(TDA7439_data[8] != 72) // if second speaker is not equals -72 dB
					TDA7439_data[8]++;
			}
			break;
		case TDA7439_MARKER_enumMAX:
			break;
	}
	if(rotate != ENCODER_ROTATE_NO)
	{
		TDA7439_I2C_Transmit();
		TDA7439_DisplayRedrawVal(0);
	}
}

void TDA7439_DisplaySignal(float left, float right)
{
	// TODO
}
