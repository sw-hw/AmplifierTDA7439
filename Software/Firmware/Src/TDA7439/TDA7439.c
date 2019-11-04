#include "TDA7439.h"
#include "../VU/vu.h"
#include <stdio.h>
#include "crc.h"

static uint8_t TDA7439_data[9];
static t_TDA7439_Marker TDA7439_marker;

static void TDA7439_I2C_Transmit(void);
static void TDA7439_TurnOn(void);
static void TDA7439_TurnOff(void);
static void TDA7439_DisplayInit(void);
static void TDA7439_DisplayShowSelector(void);
static void TDA7439_DisplayHideSelector(void);
static void TDA7439_DisplayRedrawVal(uint8_t draw_all); // 1 - draw all values; 0 - draw value according TDA7439_marker
static void TDA7439_SaveBackup(void);
static void TDA7439_ReadBackup(void);
static void TDA7439_MarkerUp(void);
static void TDA7439_MarkerDown(void);
static void TDA7439_ChangeMarkedValue(uint8_t direct);

static void TDA7439_I2C_Transmit(void)
{
	HAL_I2C_Master_Transmit(TDA7439_HI2C_INSTANCE, TDA7439_I2C_ADDRESS, TDA7439_data, sizeof(TDA7439_data), TDA7439_I2C_TIMEOUT);
}

static void TDA7439_TurnOn(void)
{
	TDA7439_marker = TDA7439_MARKER_HEAD;
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	TDA7439_ReadBackup();
	TDA7439_I2C_Transmit();
	// ---
	TDA7439_DisplayInit();
	TDA7439_DisplayRedrawVal(1);
	TDA7439_DisplayShowSelector();
	ILI9488_LedEnable();
}

static void TDA7439_TurnOff(void)
{
	ILI9488_LedDisable();
	HAL_GPIO_WritePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(OPTO_GPIO_Port, OPTO_Pin, GPIO_PIN_RESET);
	ILI9488_TurnOff(); // Call this function gives some delay and this is good
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}

static void TDA7439_DisplayInit(void)
{
	ILI9488_Init(); //initial driver setup to drive ili9488
	ILI9488_Fill_Screen(ILI9488_COLOR_BACKGROUND);
	// ===
	ILI9488_Draw_Text("Headphone:",	  TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Power amp:",	  TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Multiplexer:", TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Input gain:",  TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Volume:",	  TDA7439_LEFT_OFFSET_FIRST_COL,  TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	// ---
	ILI9488_Draw_Text("Bass gain:",	  TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Mid gain:",	  TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Treble gain:", TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("Balance:",	  TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	ILI9488_Draw_Text("VU-meter:",	  TDA7439_LEFT_OFFSET_SECOND_COL, TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_LABELS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
	// ---
	VU_Init();
}

static void TDA7439_DisplayShowSelector(void)
{
	switch(TDA7439_marker)
	{
		case TDA7439_MARKER_HEAD:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_POW_AMP:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_MULTIPLEXER:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_GAIN:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_VOLUME:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_BASS:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FIRST_ROW, 	TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_MID:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_TREBLE:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_BALANCE:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_VU:
			ILI9488_Draw_Char(TDA7439_MARK_SELECTED_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_enumMAX:
			break;
	}
}

static void TDA7439_DisplayHideSelector(void)
{
	switch(TDA7439_marker)
	{
		case TDA7439_MARKER_HEAD:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_POW_AMP:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_MULTIPLEXER:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_GAIN:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_VOLUME:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_FIRST_MARK,  TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_BASS:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FIRST_ROW, 	TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_MID:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_TREBLE:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_BALANCE:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_VU:
			ILI9488_Draw_Char(TDA7439_MARK_UNSELECT_SIMBOL, TDA7439_LEFT_OFFSET_SECOND_MARK, TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_MARK, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
			break;
		case TDA7439_MARKER_enumMAX:
			break;
	}
}

static void TDA7439_DisplayRedrawVal(uint8_t draw_all)
{
	char str[15];
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
					ILI9488_Draw_Text("ON ", TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_FIRST_ROW,  ILI9488_GREEN, 	 TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				else
					ILI9488_Draw_Text("OFF", TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_FIRST_ROW,  TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_POW_AMP:
				if(HAL_GPIO_ReadPin(OPTO_GPIO_Port, OPTO_Pin))
					ILI9488_Draw_Text("ON ",  TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_SECOND_ROW, ILI9488_GREEN,	  TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				else
					ILI9488_Draw_Text("OFF",  TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_SECOND_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_MULTIPLEXER:
				sprintf(str, "in %d", 4 - TDA7439_data[1]);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_THIRD_ROW,  TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_GAIN:
				val = (TDA7439_data[2] << 1) - (int8_t)TDA7439_data[3];
				if(val > 0)
					sprintf(str, "+%d dB  ", val);
				else
					sprintf(str, "%d dB  ", val);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_FIRST_VALS,  TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
				// ---
			case TDA7439_MARKER_VOLUME:
				sprintf(str, "%d dB  ", -(int8_t)(TDA7439_data[7] < TDA7439_data[8] ? TDA7439_data[7] : TDA7439_data[8]));
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_FIRST_VALS, TDA7439_TOP_OFFSET_FIFTH_ROW,  TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_BASS:
				TDA7439_EQ_TO_DB(TDA7439_data[4], val);
				if(val > 0)
					sprintf(str, "+%d dB  ", val);
				else
					sprintf(str, "%d dB  ", val);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_FIRST_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_MID:
				TDA7439_EQ_TO_DB(TDA7439_data[5], val);
				if(val > 0)
					sprintf(str, "+%d dB  ", val);
				else
					sprintf(str, "%d dB  ", val);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_SECOND_ROW,  TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_TREBLE:
				TDA7439_EQ_TO_DB(TDA7439_data[6], val);
				if(val > 0)
					sprintf(str, "+%d dB  ", val);
				else
					sprintf(str, "%d dB  ", val);
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_THIRD_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_BALANCE:
				val = (int8_t)TDA7439_data[7] - (int8_t)TDA7439_data[8];
				if(val == 0)
					ILI9488_Draw_Text("==      ", TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_FOURTH_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				else
				{
					if(val < 0)
						sprintf(str, "L %d dB ", val);
					else
						sprintf(str, "R %d dB ", -val);
					ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_FOURTH_ROW, ILI9488_RED, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				}
				break;
			case TDA7439_MARKER_VU:
				switch(VU_GetMode())
				{
					case VU_MODE_COLUMN_ONLY:
						sprintf(str, "col only  ");
						break;
					case VU_MODE_COLUMN_AND_PEAK:
						sprintf(str, "col & peak");
						break;
					case VU_MODE_enumMAX:
						break;
				}
				ILI9488_Draw_Text(str, TDA7439_LEFT_OFFSET_SECOND_VALS, TDA7439_TOP_OFFSET_FIFTH_ROW, TDA7439_COLOR_VALS, TDA7439_FONT_SIZE, ILI9488_COLOR_BACKGROUND);
				break;
			case TDA7439_MARKER_enumMAX:
				break;
		}
		marker++;
	} while(draw_all && marker < TDA7439_MARKER_enumMAX);
}

static void TDA7439_SaveBackup(void)
{
	HAL_PWR_EnableBkUpAccess();
	// ---
	RTC->BKP0R = *(uint32_t*)(&TDA7439_data[0]);
	RTC->BKP1R = *(uint32_t*)(&TDA7439_data[4]);
	RTC->BKP2R = TDA7439_data[8];
	// ---
	HAL_CRC_Calculate(&hcrc, (uint32_t*)(&RTC->BKP0R), 1);
	HAL_CRC_Accumulate(&hcrc, (uint32_t*)(&RTC->BKP1R), 1);
	RTC->BKP3R = HAL_CRC_Accumulate(&hcrc, (uint32_t*)(&RTC->BKP2R), 1);
}

static void TDA7439_ReadBackup(void)
{
	HAL_PWR_EnableBkUpAccess();
	// ---
	HAL_CRC_Calculate(&hcrc, (uint32_t*)(&RTC->BKP0R), 1);
	HAL_CRC_Accumulate(&hcrc, (uint32_t*)(&RTC->BKP1R), 1);
	if(HAL_CRC_Accumulate(&hcrc, (uint32_t*)(&RTC->BKP2R), 1) == RTC->BKP3R)
	{
		*(uint32_t*)(&TDA7439_data[0]) = RTC->BKP0R;
		*(uint32_t*)(&TDA7439_data[4]) = RTC->BKP1R;
		TDA7439_data[8] = RTC->BKP2R;
	}
	else
	{
		TDA7439_data[0] = 0x10; 		// start sub-address and auto increment mode
		TDA7439_data[1] = 0b11; 		// multiplexer
		TDA7439_data[2] = 0b0000;		// input gain
		TDA7439_data[3] = 0b0000; 		// volume
		TDA7439_data[4] = 0b1101; 		// bass (0b0000 to 0b0111 to 0b1000)
		TDA7439_data[5] = 0b0000; 		// mid-range (0b0000 to 0b0111 to 0b1000)
		TDA7439_data[6] = 0b1000; 		// treble (0b0000 to 0b01111 to 0b1000)
		TDA7439_data[7] = 72;			// first speaker attenuation
		TDA7439_data[8] = 72;			// second speaker attenuation
	}
}

static void TDA7439_MarkerUp(void)
{
	TDA7439_DisplayHideSelector();
	if(TDA7439_marker == (TDA7439_MARKER_enumMAX - 1))
		TDA7439_marker = 0;
	else
		TDA7439_marker++;
	TDA7439_DisplayShowSelector();
}

static void TDA7439_MarkerDown(void)
{
	TDA7439_DisplayHideSelector();
	if(TDA7439_marker == 0)
		TDA7439_marker = (TDA7439_MARKER_enumMAX - 1);
	else
		TDA7439_marker--;
	TDA7439_DisplayShowSelector();
}

static void TDA7439_ChangeMarkedValue(uint8_t direct)
{
	switch(TDA7439_marker)
	{
		case TDA7439_MARKER_HEAD:
			if(direct)
				HAL_GPIO_WritePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin, GPIO_PIN_SET);
			else
				HAL_GPIO_WritePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin, GPIO_PIN_RESET);
			break;
		case TDA7439_MARKER_POW_AMP:
			if(direct)
				HAL_GPIO_WritePin(OPTO_GPIO_Port, OPTO_Pin, GPIO_PIN_SET);
			else
				HAL_GPIO_WritePin(OPTO_GPIO_Port, OPTO_Pin, GPIO_PIN_RESET);
			break;
		case TDA7439_MARKER_MULTIPLEXER:
			if(direct)
			{
				if(TDA7439_data[1] == 0)
					TDA7439_data[1] = 0x03;
				else
					TDA7439_data[1]--;
			}
			else
			{
				if(TDA7439_data[1] == 0x03)
					TDA7439_data[1] = 0;
				else
					TDA7439_data[1]++;
			}
			break;
		case TDA7439_MARKER_GAIN:
			if(direct)
			{
				if(TDA7439_data[3] == 0) // if volume equals 0 dB
				{
					if(TDA7439_data[2] != 0x0F) // if gain not equals +30 dB
						TDA7439_data[2]++;
				}
				else
					TDA7439_data[3]--;
			}
			else
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
			if(direct)
			{
				if(TDA7439_data[7] != 0 && TDA7439_data[8] != 0) // if both speaker attenuates are not equals 0 dB
				{
					TDA7439_data[7]--;
					TDA7439_data[8]--;
				}
			}
			else
			{
				if(TDA7439_data[7] != 72 && TDA7439_data[8] != 72) // if both speaker attenuates are not equals -72 dB
				{
					TDA7439_data[7]++;
					TDA7439_data[8]++;
				}
			}
			break;
		case TDA7439_MARKER_BASS:
			if(direct)
				TDA7439_EQ_INC(TDA7439_data[4])
			else
				TDA7439_EQ_DEC(TDA7439_data[4])
			break;
		case TDA7439_MARKER_MID:
			if(direct)
				TDA7439_EQ_INC(TDA7439_data[5])
			else
				TDA7439_EQ_DEC(TDA7439_data[5])
			break;
		case TDA7439_MARKER_TREBLE:
			if(direct)
				TDA7439_EQ_INC(TDA7439_data[6])
			else
				TDA7439_EQ_DEC(TDA7439_data[6])
			break;
		case TDA7439_MARKER_BALANCE:
			if(direct)
			{
				if(TDA7439_data[7] > TDA7439_data[8])
					TDA7439_data[7]--;
				else if(TDA7439_data[8] != 72) // if speaker is not equals -72 dB
					TDA7439_data[8]++;
			}
			else
			{
				if(TDA7439_data[8] > TDA7439_data[7])
					TDA7439_data[8]--;
				else if(TDA7439_data[7] != 72) // if speaker is not equals -72 dB
					TDA7439_data[7]++;
			}
			break;
		case TDA7439_MARKER_VU:
			if(direct)
				VU_NextMode();
			else
				VU_PrevMode();
			break;
		case TDA7439_MARKER_enumMAX:
			break;
	}
	TDA7439_I2C_Transmit();
	TDA7439_DisplayRedrawVal(0);
	TDA7439_SaveBackup();
}

// === Interface functions ===

void TDA7439_EncoderButton(uint8_t state)
{
	static uint32_t last_time = 0;
	static uint8_t enable = 0;
	if(HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin))
	{
		static uint8_t short_push = 0;
		if(state)
		{
			last_time = HAL_GetTick();
			enable = 1;
			if(short_push)
			{
				short_push = 0;
				TDA7439_MarkerUp();
			}
		}
		else if(enable)
		{
			if(HAL_GetTick() - last_time > TDA7439_TIME_TURN_OFF)
			{
				TDA7439_TurnOff();
				short_push = 0;
				enable = 0;
			}
			else
				short_push = 1;
		}
	}
	else
	{
		if(state)
		{
			last_time = HAL_GetTick();
			enable = 1;
		}
		else if(enable && HAL_GetTick() - last_time > TDA7439_TIME_TURN_ON)
		{
			TDA7439_TurnOn();
			enable = 0;
		}
	}
}

void TDA7439_EncoderRotate(EncoderRotate_t rotate)
{
	if(!HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin))
		return;
	switch(rotate)
	{
		case ENCODER_ROTATE_L:
			TDA7439_ChangeMarkedValue(0);
			break;
		case ENCODER_ROTATE_R:
			TDA7439_ChangeMarkedValue(1);
			break;
		default:
			break;
	}
}

void	TDA7439_ButtonCode(int16_t code)
{
	static uint32_t time_change_power_state = 0;
	static uint8_t	enable_change_power_state = 1;
	if(!enable_change_power_state && HAL_GetTick() - time_change_power_state > TDA7439_TIME_BUT_POWER)
		enable_change_power_state = 1;
	switch(code)
	{
		case TDA7439_BUTCODE_POWER:
			if(enable_change_power_state)
			{
				if(HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin))
					TDA7439_TurnOff();
				else
					TDA7439_TurnOn();
				time_change_power_state = HAL_GetTick();
				enable_change_power_state = 0;
			}
			break;
		case TDA7439_BUTCODE_MARKER_UP:
			TDA7439_MarkerUp();
			break;
		case TDA7439_BUTCODE_MARKER_DOWN:
			TDA7439_MarkerDown();
			break;
		case TDA7439_BUTCODE_VAL_UP:
			TDA7439_ChangeMarkedValue(1);
			break;
		case TDA7439_BUTCODE_VAL_DOWN:
			TDA7439_ChangeMarkedValue(0);
			break;
		default:
			break;
	}
}

uint8_t	TDA7439_GetAmplifierState(void)
{
	return HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin);
}
