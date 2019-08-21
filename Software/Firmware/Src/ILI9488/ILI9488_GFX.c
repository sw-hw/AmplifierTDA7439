#include "ILI9488_STM32_Driver.h"
#include "ILI9488_GFX.h"
//#include "5x5_font.h"
#include "font_droid_sans_mono_8x16_utf8.h"
#include "spi.h"

/*Draw hollow circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9488_Draw_Hollow_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	int x = Radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (Radius << 1);

    while (x >= y)
    {
        ILI9488_Draw_Pixel(X + x, Y + y, Colour);
        ILI9488_Draw_Pixel(X + y, Y + x, Colour);
        ILI9488_Draw_Pixel(X - y, Y + x, Colour);
        ILI9488_Draw_Pixel(X - x, Y + y, Colour);
        ILI9488_Draw_Pixel(X - x, Y - y, Colour);
        ILI9488_Draw_Pixel(X - y, Y - x, Colour);
        ILI9488_Draw_Pixel(X + y, Y - x, Colour);
        ILI9488_Draw_Pixel(X + x, Y - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-Radius << 1) + dx;
        }
    }
}

/*Draw filled circle at X,Y location with specified radius and colour. X and Y represent circles center */
void ILI9488_Draw_Filled_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
	
		int x = Radius;
    int y = 0;
    int xChange = 1 - (Radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = X - x; i <= X + x; i++)
        {
            ILI9488_Draw_Pixel(i, Y + y,Colour);
            ILI9488_Draw_Pixel(i, Y - y,Colour);
        }
        for (int i = X - y; i <= X + y; i++)
        {
            ILI9488_Draw_Pixel(i, Y + x,Colour);
            ILI9488_Draw_Pixel(i, Y - x,Colour);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
		//Really slow implementation, will require future overhaul
		//TODO:	https://stackoverflow.com/questions/1201200/fast-algorithm-for-drawing-filled-circles	
}

/*Draw a hollow rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9488_Draw_Hollow_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;
	float 		Calc_Negative = 0;
	
	Calc_Negative = X1 - X0;
	if(Calc_Negative < 0) Negative_X = 1;
	Calc_Negative = 0;
	
	Calc_Negative = Y1 - Y0;
	if(Calc_Negative < 0) Negative_Y = 1;
	
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;		
	}
	else
	{
		X_length = X0 - X1;		
	}
	ILI9488_Draw_Horizontal_Line(X0, Y0, X_length, Colour);
	ILI9488_Draw_Horizontal_Line(X0, Y1, X_length, Colour);
	
	
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;		
	}
	ILI9488_Draw_Vertical_Line(X0, Y0, Y_length, Colour);
	ILI9488_Draw_Vertical_Line(X1, Y0, Y_length, Colour);
	
	if((X_length > 0)||(Y_length > 0)) 
	{
		ILI9488_Draw_Pixel(X1, Y1, Colour);
	}
	
}

/*Draw a filled rectangle between positions X0,Y0 and X1,Y1 with specified colour*/
void ILI9488_Draw_Filled_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
	uint16_t 	X_length = 0;
	uint16_t 	Y_length = 0;
	uint8_t		Negative_X = 0;
	uint8_t 	Negative_Y = 0;
	int32_t 	Calc_Negative = 0;
	
	uint16_t X0_true = 0;
	uint16_t Y0_true = 0;
	
	Calc_Negative = X1 - X0;
	if(Calc_Negative < 0) Negative_X = 1;
	Calc_Negative = 0;
	
	Calc_Negative = Y1 - Y0;
	if(Calc_Negative < 0) Negative_Y = 1;
	
	
	//DRAW HORIZONTAL!
	if(!Negative_X)
	{
		X_length = X1 - X0;
		X0_true = X0;
	}
	else
	{
		X_length = X0 - X1;
		X0_true = X1;
	}
	
	//DRAW VERTICAL!
	if(!Negative_Y)
	{
		Y_length = Y1 - Y0;
		Y0_true = Y0;		
	}
	else
	{
		Y_length = Y0 - Y1;
		Y0_true = Y1;	
	}
	
	ILI9488_Draw_Rectangle(X0_true, Y0_true, X_length, Y_length, Colour);
}

/*Draws a character (fonts imported from fonts.h) at X,Y location with specified font colour, size and Background colour*/
/*See fonts.h implementation of font on what is required for changing to a different font when switching fonts libraries*/
void ILI9488_Draw_Char(char Character, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
	const uint8_t function_char = Character - 32;
    uint8_t i,j;
    uint16_t temp[CHAR_WIDTH];
	for(j = 0; j < CHAR_WIDTH; j++)
		temp[j] = font_droid_sans_mono_8x16_utf8_32_127_data[function_char * CHAR_WIDTH + j]
			+ (font_droid_sans_mono_8x16_utf8_32_127_data[
					(function_char + FONT_DROID_SANS_MONO_8X16_UTF8_32_127_SIMBOLS) * CHAR_WIDTH + j
					] << 8);
    // Draw pixels
	ILI9488_Draw_Rectangle(X, Y, CHAR_WIDTH * Size, CHAR_HEIGHT * Size, Background_Colour);
    for (j = 0; j < CHAR_WIDTH; j++) {
        for (i = 0; i < CHAR_HEIGHT; i++) {
            if (temp[j] & (1 << i)) {
				if(Size == 1)
					ILI9488_Draw_Pixel(X + j, Y + i, Colour);
				else
					ILI9488_Draw_Rectangle(X + (j * Size), Y + (i * Size), Size, Size, Colour);
            }
        }
    }
}

/*Draws an array of characters (fonts imported from fonts.h) at X,Y location with specified font colour, size and Background colour*/
/*See fonts.h implementation of font on what is required for changing to a different font when switching fonts libraries*/
void ILI9488_Draw_Text(const char* Text, uint16_t X, uint16_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
    while (*Text) {
        ILI9488_Draw_Char(*Text++, X, Y, Colour, Size, Background_Colour);
        X += CHAR_WIDTH*Size;
    }
}
