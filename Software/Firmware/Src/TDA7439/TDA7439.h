#ifndef TDA7439_TDA7439_H_
#define TDA7439_TDA7439_H_

#include "../ILI9488/ILI9488_STM32_Driver.h"
#include "../ILI9488/ILI9488_GFX.h"

#include "i2c.h"

// === configuration ===
#define TDA7439_HI2C_INSTANCE				&hi2c1
#define TDA7439_I2C_ADDRESS 				0x88
#define TDA7439_I2C_TIMEOUT					1000UL
#define TDA7439_TIME_TURN_ON				1000UL
#define TDA7439_TIME_TURN_OFF				2000UL
// ---
#define TDA7439_LEFT_OFFSET_FIRST_COL		30
#define TDA7439_LEFT_OFFSET_SECOND_COL		270
#define TDA7439_TOP_OFFSET_FIRST_ROW		10
#define TDA7439_TOP_OFFSET_SECOND_ROW		30
#define TDA7439_TOP_OFFSET_THIRD_ROW		50
#define TDA7439_TOP_OFFSET_FOURTH_ROW		70
#define TDA7439_TOP_OFFSET_FIFTH_ROW		90
#define	TDA7439_COLOR_LABELS				ILI9488_WHITE
#define	TDA7439_FONT_SIZE					1
#define	TDA7439_LEFT_OFFSET_FIRST_VALS		140
#define	TDA7439_LEFT_OFFSET_SECOND_VALS		380
#define	TDA7439_COLOR_VALS					ILI9488_WHITE
#define	TDA7439_LEFT_OFFSET_FIRST_MARK		10
#define TDA7439_LEFT_OFFSET_SECOND_MARK		250
#define	TDA7439_MARK_UNSELECT_SIMBOL		' '
#define	TDA7439_MARK_SELECTED_SIMBOL		'>'
#define	TDA7439_COLOR_MARK					ILI9488_ORANGE
// ---
// TODO
#define	TDA7439_BUTCODE_POWER				0x00
#define	TDA7439_BUTCODE_MARKER_UP			0x01
#define	TDA7439_BUTCODE_MARKER_DOWN			0x02
#define	TDA7439_BUTCODE_VAL_UP				0x03
#define	TDA7439_BUTCODE_VAL_DOWN			0x04
#define	TDA7439_TIME_BUT_POWER				5000UL
// ==============

typedef enum {
	ENCODER_ROTATE_NO = 0,
	ENCODER_ROTATE_R,
	ENCODER_ROTATE_L
} EncoderRotate_t;

typedef enum {
	TDA7439_MARKER_HEAD = 0,	// Headphone amplifier
	TDA7439_MARKER_POW_AMP,		// Power amplifier (switched by opto relay)
	TDA7439_MARKER_MULTIPLEXER,
	TDA7439_MARKER_GAIN,
	TDA7439_MARKER_VOLUME,
	TDA7439_MARKER_BASS,
	TDA7439_MARKER_MID,
	TDA7439_MARKER_TREBLE,
	TDA7439_MARKER_BALANCE,
	TDA7439_MARKER_VU,
	TDA7439_MARKER_enumMAX
} t_TDA7439_Marker;

// === Interface functions ===
void 	TDA7439_EncoderButton(uint8_t state);				// 0 - push down; 1 - push up
void 	TDA7439_EncoderRotate(EncoderRotate_t rotate);
void	TDA7439_ButtonCode(int16_t code);
uint8_t	TDA7439_GetAmplifierState(void);					// 0 - OFF; 1 - ON
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
