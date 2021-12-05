#ifndef ILI9488_STM32_DRIVER_H
#define ILI9488_STM32_DRIVER_H

#include <stdint.h>


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

// Color definitions
#define ILI9488_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9488_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9488_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9488_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9488_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9488_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9488_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9488_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9488_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9488_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9488_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9488_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9488_RED         0xF800      /* 255,   0,   0 */
#define ILI9488_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9488_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9488_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9488_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9488_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9488_PINK        0xF81F

#define ILI9488_COLOR_BACKGROUND	ILI9488_BLACK

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
void ILI9488_TurnOff(void);
void ILI9488_Fill_Screen(uint16_t Colour);
void ILI9488_Draw_Pixel(uint16_t X, uint16_t Y, uint16_t Colour);
void ILI9488_Draw_Colour_Burst(uint16_t Colour, uint32_t Size);
void ILI9488_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour);
void ILI9488_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour);
void ILI9488_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour);
	
#endif
