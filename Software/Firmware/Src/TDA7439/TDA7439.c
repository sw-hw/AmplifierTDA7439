#include "TDA7439.h"

static uint8_t TDA7439_data[9];

static void TDA7439_I2C_Transmit()
{
	HAL_I2C_Master_Transmit(HI2C_INSTANCE, 0x0088, TDA7439_data, 9, 1000);
}

void TDA7439_Init(void)
{
	TDA7439_data[0] = 0x10; 		// start subaddress and auto increment mode
	TDA7439_data[1] = 0b11; 		// multiplexer
	TDA7439_data[2] = 0b0000;		// input gain
	TDA7439_data[3] = 0b0000; 		// volume
	TDA7439_data[4] = 0b1101; 		// bass (0b0000 to 0b0111 to 0b1000)
	TDA7439_data[5] = 0b0000; 		// mid-range (0b0000 to 0b0111 to 0b1000)
	TDA7439_data[6] = 0b1000; 		// treble (0b0000 to 0b01111 to 0b1000)
	TDA7439_data[7] = 0b0000000;	// first speaker attenuation
	TDA7439_data[8] = 0b0000000;	// second speaker attenuation
	TDA7439_I2C_Transmit();
}
