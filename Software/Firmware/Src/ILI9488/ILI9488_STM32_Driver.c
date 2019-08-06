/* Includes ------------------------------------------------------------------*/
#include "ILI9488_STM32_Driver.h"
#include "spi.h"
#include "gpio.h"

/* Global Variables ------------------------------------------------------------------*/
volatile uint16_t LCD_HEIGHT = ILI9488_SCREEN_HEIGHT;
volatile uint16_t LCD_WIDTH	 = ILI9488_SCREEN_WIDTH;

/*Send data (char) to LCD*/
void ILI9488_SPI_Send(uint8_t SPI_Data)
{
	// TODO it needs to rewrite without HAL
	HAL_SPI_Transmit(HSPI_INSTANCE, &SPI_Data, 1, 1);
}

/* Send command (char) to LCD */
void ILI9488_Write_Command(uint8_t Command)
{
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	ILI9488_SPI_Send(Command);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/* Send Data (char) to LCD */
void ILI9488_Write_Data(uint8_t Data)
{
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	ILI9488_SPI_Send(Data);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/* Set Address - Location block - to draw into */
void ILI9488_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2)
{
	/* TODO
	 * Попробовать убрать второй блок и/или попробовать убрать передачу конечной координаты
	 */

	ILI9488_Write_Command(0x2A);
	ILI9488_Write_Data(X1 >> 8);
	ILI9488_Write_Data(X1);
	ILI9488_Write_Data(X2 >> 8);
	ILI9488_Write_Data(X2);

	ILI9488_Write_Command(0x2B);
	ILI9488_Write_Data(Y1 >> 8);
	ILI9488_Write_Data(Y1);
	ILI9488_Write_Data(Y2 >> 8);
	ILI9488_Write_Data(Y2);

	ILI9488_Write_Command(0x2C);
}

/*Ser rotation of the screen - changes x0 and y0*/
void ILI9488_Set_Rotation(uint8_t Rotation)
{
	uint8_t screen_rotation = Rotation;
	ILI9488_Write_Command(0x36);
	HAL_Delay(1);
	switch(screen_rotation)
	{
		case SCREEN_VERTICAL_1:
			ILI9488_Write_Data(0x40 | 0x08);
			LCD_WIDTH = ILI9488_SCREEN_HEIGHT;
			LCD_HEIGHT = ILI9488_SCREEN_WIDTH;
			break;
		case SCREEN_HORIZONTAL_1:
			ILI9488_Write_Data(0x20 | 0x08);
			LCD_WIDTH  = ILI9488_SCREEN_WIDTH;
			LCD_HEIGHT = ILI9488_SCREEN_HEIGHT;
			break;
		case SCREEN_VERTICAL_2:
			ILI9488_Write_Data(0x80 | 0x08);
			LCD_WIDTH  = ILI9488_SCREEN_HEIGHT;
			LCD_HEIGHT = ILI9488_SCREEN_WIDTH;
			break;
		case SCREEN_HORIZONTAL_2:
			ILI9488_Write_Data(0x40 | 0x80 | 0x20 | 0x08);
			LCD_WIDTH  = ILI9488_SCREEN_WIDTH;
			LCD_HEIGHT = ILI9488_SCREEN_HEIGHT;
			break;
		default:
			//EXIT IF SCREEN ROTATION NOT VALID!
			break;
	}
}

/*Enable LCD led*/
void ILI9488_LedEnable(void)
{
	HAL_GPIO_WritePin(LCD_LED_PORT, LCD_LED_PIN, GPIO_PIN_SET);
}

/*Disable LCD led*/
void ILI9488_LedDisable(void)
{
	HAL_GPIO_WritePin(LCD_LED_PORT, LCD_LED_PIN, GPIO_PIN_RESET);
}

void init_ILI9488(void)
{
	ILI9488_Write_Command(0xE0);
	ILI9488_Write_Data(0x00);
	ILI9488_Write_Data(0x03);
	ILI9488_Write_Data(0x09);
	ILI9488_Write_Data(0x08);
	ILI9488_Write_Data(0x16);
	ILI9488_Write_Data(0x0A);
	ILI9488_Write_Data(0x3F);
	ILI9488_Write_Data(0x78);
	ILI9488_Write_Data(0x4C);
	ILI9488_Write_Data(0x09);
	ILI9488_Write_Data(0x0A);
	ILI9488_Write_Data(0x08);
	ILI9488_Write_Data(0x16);
	ILI9488_Write_Data(0x1A);
	ILI9488_Write_Data(0x0F);

	ILI9488_Write_Command(0XE1);
	ILI9488_Write_Data(0x00);
	ILI9488_Write_Data(0x16);
	ILI9488_Write_Data(0x19);
	ILI9488_Write_Data(0x03);
	ILI9488_Write_Data(0x0F);
	ILI9488_Write_Data(0x05);
	ILI9488_Write_Data(0x32);
	ILI9488_Write_Data(0x45);
	ILI9488_Write_Data(0x46);
	ILI9488_Write_Data(0x04);
	ILI9488_Write_Data(0x0E);
	ILI9488_Write_Data(0x0D);
	ILI9488_Write_Data(0x35);
	ILI9488_Write_Data(0x37);
	ILI9488_Write_Data(0x0F);

	ILI9488_Write_Command(0xC0);      //Power Control 1
	ILI9488_Write_Data(0x17);    //Vreg1out
	ILI9488_Write_Data(0x15);    //Verg2out

	ILI9488_Write_Command(0xC1);      //Power Control 2
	ILI9488_Write_Data(0x41);    //VGH,VGL

	ILI9488_Write_Command(0xC5);      //Power Control 3
	ILI9488_Write_Data(0x00);
	ILI9488_Write_Data(0x12);    //Vcom
	ILI9488_Write_Data(0x80);

	ILI9488_Write_Command(0x36);      //Memory Access
	ILI9488_Write_Data(0x48);

	ILI9488_Write_Command(0x3A);      // Interface Pixel Format
	//ILI9488_Write_Data(0x66); 	  //18 bit
	ILI9488_Write_Data(0x51); 	  	  // DPI 16 bits/pixel; DBI 3 bits/pixel (8 color)

	ILI9488_Write_Command(0xB0);      // Interface Mode Control
	ILI9488_Write_Data(0x80);     			 //SDO NOT USE

	//ILI9488_Write_Command(0xB1);      //Frame Rate Control (In Normal Mode/Full Colors)
	//ILI9488_Write_Data(0xA0);    //60Hz

	// TODO if it won't help, try use init function from https://www.avrfreaks.net/sites/default/files/forum_attachments/ili9488.c
	ILI9488_Write_Command(0xB2);	// Frame Rate Control (In Idle Mode/8 Colors)
	ILI9488_Write_Data(0x03);		// fosc/8
	ILI9488_Write_Data(0x1F);		// 31 clock per Line

	ILI9488_Write_Command(0xB4);      //Display Inversion Control
	ILI9488_Write_Data(0x02);    //2-dot

	ILI9488_Write_Command(0xB6);      //Display Function Control  RGB/MCU Interface Control
	ILI9488_Write_Data(0x02);    //MCU
	ILI9488_Write_Data(0x02);    //Source,Gate scan dieection

	ILI9488_Write_Command(0xE9);      // Set Image Functio
	ILI9488_Write_Data(0x00);    // Disable 24 bit data

	ILI9488_Write_Command(0xF7);      // Adjust Control
	ILI9488_Write_Data(0xA9);
	ILI9488_Write_Data(0x51);
	ILI9488_Write_Data(0x2C);
	ILI9488_Write_Data(0x82);    // D7 stream, loose

	ILI9488_Write_Command(0x11);    //Exit Sleep
	HAL_Delay(120);
	ILI9488_Write_Command(0x29);    //Display on
}

/*Initialize LCD display*/
void ILI9488_Init(void)
{
	init_ILI9488();
	HAL_Delay(500);
	ILI9488_Set_Rotation(SCREEN_VERTICAL_1);
	ILI9488_LedEnable();
}

//FILL THE ENTIRE SCREEN WITH SELECTED COLOUR
/*Sets address (entire screen) and Sends Height*Width ammount of colour information to LCD*/
void ILI9488_Fill_Screen(uint8_t Colour)
{
	ILI9488_Set_Address(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
	ILI9488_Draw_Colour_Burst(Colour, LCD_WIDTH_x_LCD_HEIGHT);
}

//INTERNAL FUNCTION OF LIBRARY
/*Sends block colour information to LCD*/
void ILI9488_Draw_Colour_Burst(uint8_t Colour, uint32_t Size)
{
	//SENDS COLOUR
	Size = Size >> 1;
	uint32_t Buffer_Size = Size;
	if(Buffer_Size > BURST_MAX_SIZE)
		Buffer_Size = BURST_MAX_SIZE;
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	uint8_t byte = Colour & 0x07;
	byte = 0x80 | (byte << 3) | byte;
	unsigned char burst_buffer[Buffer_Size];
	for(uint32_t j = 0; j < Buffer_Size; j++)
		burst_buffer[j] = byte;
	uint32_t Sending_in_Block = Size / Buffer_Size;
	uint32_t Remainder_from_block = Size % Buffer_Size;
	for(uint32_t j = 0; j < (Sending_in_Block); j++)
		HAL_SPI_Transmit(HSPI_INSTANCE, (unsigned char *)burst_buffer, Buffer_Size, 10);
	//REMAINDER!
	HAL_SPI_Transmit(HSPI_INSTANCE, (unsigned char *)burst_buffer, Remainder_from_block, 10);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

//DRAW PIXEL AT XY POSITION WITH SELECTED COLOUR
//
//Location is dependant on screen orientation. x0 and y0 locations change with orientations.
//Using pixels to draw big simple structures is not recommended as it is really slow
//Try using either rectangles or lines if possible
//
void ILI9488_Draw_Pixel(uint16_t X, uint16_t Y, uint8_t Colour)
{
	ILI9488_Set_Address(X, Y, X, Y);
	HAL_Delay(1000);
	//*
	//COLOUR
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
	for(uint16_t i = 0; i < 0xFFFF; i++)
		ILI9488_SPI_Send(0x80 | Colour);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
	//*/
}

//DRAW RECTANGLE OF SET SIZE AND HEIGTH AT X and Y POSITION WITH CUSTOM COLOUR
//
//Rectangle is hollow. X and Y positions mark the upper left corner of rectangle
//As with all other draw calls x0 and y0 locations dependant on screen orientation
//
void ILI9488_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint8_t Colour)
{
	ILI9488_Set_Address(X, Y, X+Width-1, Y+Height-1);
	ILI9488_Draw_Colour_Burst(Colour, Height*Width);
}

//DRAW LINE FROM X,Y LOCATION to X+Width,Y LOCATION
void ILI9488_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint8_t Colour)
{
	ILI9488_Set_Address(X, Y, X+Width-1, Y);
	ILI9488_Draw_Colour_Burst(Colour, Width);
}

//DRAW LINE FROM X,Y LOCATION to X,Y+Height LOCATION
void ILI9488_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint8_t Colour)
{
	ILI9488_Set_Address(X, Y, X, Y+Height-1);
	ILI9488_Draw_Colour_Burst(Colour, Height);
}
