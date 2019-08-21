#ifndef TDA7439_TDA7439_H_
#define TDA7439_TDA7439_H_

#include "../ILI9488/ILI9488_STM32_Driver.h"
#include "../ILI9488/ILI9488_GFX.h"

#include "i2c.h"

// --- configuration ---
#define TDA7439_HI2C_INSTANCE				&hi2c1
#define TDA7439_I2C_ADDRESS 				0x88
#define TDA7439_I2C_TIMEOUT					1000UL
#define TDA7439_TIME_TURN_ON				1000UL
#define TDA7439_TIME_TURN_OFF				2000UL
#define TDA7439_DISPLAY_COLOR_BACKGROUND	ILI9488_BLACK
// ==============

typedef enum {
	TDA7439_MARKER_HEAD = 0,	// Headphone amplifier
	TDA7439_MARKER_POW_AMP,		// Power amplifier (switched by opto relay)
	TDA7439_MARKER_MULTIPLEXER,
	TDA7439_MARKER_GAIN,
	TDA7439_MARKER_VOLUME,
	TDA7439_MARKER_BASS,
	TDA7439_MARKER_MID,
	TDA7439_MARKER_TREBLE
} t_TDA7439_Marker;

// --- interface ---
void TDA7439_EncoderButton(uint8_t state);	// 0 - push down; 1 - push up
void TDA7439_EncoderRotate(uint8_t inc); 	// 0 - decrement; 1 - increment
// =================

// === macros for bass, mid, treble ===
#define TDA7439_EQ_INC(value) { 		\
	if(value == 0x07)					\
		value = 0x0E;					\
	else if(value < 0x07)				\
		value++;						\
	else if(value > 0x08)				\
		value--;			 			\
}
#define TDA7439_EQ_DEC(value) { 		\
	if(value == 0x0E)					\
		value = 0x07;					\
	else if(value > 0x07)				\
		value++;						\
	else if(value > 0x00)				\
		value--;						\
}
#define TDA7439_EQ_TO_DB(value, db) { 	\
	if(value < 0x08)					\
		db = (value - 0x07L);			\
	else								\
		db = (0x0FL - value);			\
	db *= 2;							\
}
// ====================================

#endif /* TDA7439_TDA7439_H_ */
