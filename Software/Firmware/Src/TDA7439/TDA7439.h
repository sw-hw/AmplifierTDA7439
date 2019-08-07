#ifndef TDA7439_TDA7439_H_
#define TDA7439_TDA7439_H_

#include "i2c.h"

// I2C INSTANCE
#define HI2C_INSTANCE		&hi2c1
#define TDA7439_I2C_ADDRESS 0x88

void TDA7439_Init(void);

#endif /* TDA7439_TDA7439_H_ */
