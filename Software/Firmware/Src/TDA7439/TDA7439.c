#include "TDA7439.h"

static uint8_t TDA7439_data[9];
static t_TDA7439_Marker TDA7439_marker;

static void TDA7439_TurnOn(void);
static void TDA7439_TurnOff(void);
static void TDA7439_DisplayInit(void);
static void TDA7439_DisplayRedrawSelector(void);
static void TDA7439_DisplayRedrawVal(uint8_t draw_all); // 1 - draw all values; 0 - draw value according TDA7439_marker

static void TDA7439_I2C_Transmit()
{
	HAL_I2C_Master_Transmit(TDA7439_HI2C_INSTANCE, TDA7439_I2C_ADDRESS, TDA7439_data, sizeof(TDA7439_data), TDA7439_I2C_TIMEOUT);
}

static void TDA7439_TurnOn(void)
{
	TDA7439_marker = TDA7439_MARKER_HEAD;
	HAL_GPIO_WritePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	TDA7439_DisplayInit();
	HAL_Delay(1000);
	TDA7439_data[0] = 0x10; 		// start sub-address and auto increment mode
	TDA7439_data[1] = 0b11; 		// multiplexer
	TDA7439_data[2] = 0b0000;		// input gain
	TDA7439_data[3] = 0b0000; 		// volume
	TDA7439_data[4] = 0b1101; 		// bass (0b0000 to 0b0111 to 0b1000)
	TDA7439_data[5] = 0b0000; 		// mid-range (0b0000 to 0b0111 to 0b1000)
	TDA7439_data[6] = 0b1000; 		// treble (0b0000 to 0b01111 to 0b1000)
	TDA7439_data[7] = 0b0000000;	// first speaker attenuation
	TDA7439_data[8] = 0b0000000;	// second speaker attenuation
	TDA7439_I2C_Transmit();
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
				if(TDA7439_marker == TDA7439_MARKER_TREBLE)
					TDA7439_marker = TDA7439_MARKER_HEAD;
				else
					TDA7439_marker++;
				TDA7439_DisplayRedrawSelector();
			}
		}
		else if(last_time != (~ 0UL) && HAL_GetTick() - last_time > TDA7439_TIME_TURN_OFF)
		{
			TDA7439_TurnOff();
			last_time = ~ 0UL;
			short_push = 0;
		}
		else
			short_push = 1;
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

void TDA7439_EncoderRotate(uint8_t inc)
{
	if(!HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin))
		return;
	switch(TDA7439_marker)
	{
		case TDA7439_MARKER_HEAD:
			HAL_GPIO_TogglePin(POW_HEAD_GPIO_Port, POW_HEAD_Pin);
			break;
		case TDA7439_MARKER_MULTIPLEXER:
			if(inc)
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
			if(inc)
			{
				if(TDA7439_data[2] != 0x0F)
					TDA7439_data[2]++;
			}
			else
			{
				if(TDA7439_data[2] != 0)
					TDA7439_data[2]--;
			}
			break;
		case TDA7439_MARKER_VOLUME:
			if(inc)
			{
				if(TDA7439_data[3] != 0) // 0 dB
					TDA7439_data[3]--;
			}
			else
			{
				if(TDA7439_data[3] != 40) // -40 dB
					TDA7439_data[3]++;
			}
			break;
		case TDA7439_MARKER_BASS:
			if(inc)
				TDA7439_EQ_INC(TDA7439_data[4])
			else
				TDA7439_EQ_DEC(TDA7439_data[4])
			break;
		case TDA7439_MARKER_MID:
			if(inc)
				TDA7439_EQ_INC(TDA7439_data[5])
			else
				TDA7439_EQ_DEC(TDA7439_data[5])
			break;
		case TDA7439_MARKER_TREBLE:
			if(inc)
				TDA7439_EQ_INC(TDA7439_data[6])
			else
				TDA7439_EQ_DEC(TDA7439_data[6])
			break;
	}
	TDA7439_I2C_Transmit();
	TDA7439_DisplayRedrawVal(0);
}

static void TDA7439_DisplayInit(void)
{
	ILI9488_Init(); //initial driver setup to drive ili9488
	ILI9488_Fill_Screen(TDA7439_DISPLAY_COLOR_BACKGROUND);
	ILI9488_LedEnable();
	// TODO
}

static void TDA7439_DisplayRedrawSelector(void)
{
	// TODO
}

static void TDA7439_DisplayRedrawVal(uint8_t draw_all)
{
	// TODO
}
