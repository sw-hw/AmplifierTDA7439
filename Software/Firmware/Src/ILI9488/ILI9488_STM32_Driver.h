#ifndef ILI9488_STM32_DRIVER_H
#define ILI9488_STM32_DRIVER_H

#include "stm32f1xx_hal.h"
#include "spi.h"

#define ILI9488_SCREEN_HEIGHT 	320
#define ILI9488_SCREEN_WIDTH 	480

#define LCD_WIDTH_x_LCD_HEIGHT (ILI9488_SCREEN_HEIGHT * ILI9488_SCREEN_WIDTH)

//SPI INSTANCE
#define HSPI_INSTANCE							&hspi2

//CHIP SELECT PIN AND PORT, STANDARD GPIO
#define LCD_CS_PORT								DISPLAY_CS_GPIO_Port
#define LCD_CS_PIN								DISPLAY_CS_Pin

//DATA COMMAND PIN AND PORT, STANDARD GPIO
#define LCD_DC_PORT								DISPLAY_DC_RS_GPIO_Port
#define LCD_DC_PIN								DISPLAY_DC_RS_Pin

//LED PIN AND PORT, STANDARD GPIO
#define	LCD_LED_PORT							DISPLAY_LED_GPIO_Port
#define	LCD_LED_PIN								DISPLAY_LED_Pin

#define BURST_MAX_SIZE 	500

#define	ILI9488_BLACK	0x00
#define	ILI9488_BLUE	0x01
#define	ILI9488_GREEN	0x02
#define ILI9488_CYAN	0x03
#define	ILI9488_RED		0x04
#define	ILI9488_MAGENTA	0x05
#define	ILI9488_YELLOW	0x06
#define	ILI9488_WHITE	0x07

#define SCREEN_VERTICAL_1		0
#define SCREEN_HORIZONTAL_1		1
#define SCREEN_VERTICAL_2		2
#define SCREEN_HORIZONTAL_2		3

void ILI9488_SPI_Send(uint8_t SPI_Data);
void ILI9488_Write_Command(uint8_t Command);
void ILI9488_Write_Data(uint8_t Data);
void ILI9488_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2);
void ILI9488_Set_Rotation(uint8_t Rotation);
void ILI9488_LedEnable(void);
void ILI9488_LedDisable(void);
void ILI9488_Init(void);
void ILI9488_Fill_Screen(uint8_t Colour);
void ILI9341_Draw_Pixel(uint16_t X, uint16_t Y, uint8_t Colour);
void ILI9341_Draw_Colour_Burst(uint8_t Colour, uint32_t Size);


void ILI9341_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour);
void ILI9341_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour);
void ILI9341_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour);
	
#endif

