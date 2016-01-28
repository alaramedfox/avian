/*	Popcorn Kernel
 *	  File:		colordef.h
 *	  Purpose:	Define all the color codes
 */
 
/* Define all possible colors in hex */
#define C_BLACK  	0x0
#define C_BLUE  	0x1
#define C_GREEN  	0x2
#define C_CYAN  	0x3
#define C_RED  	0x4
#define C_MAGENTA  	0x5
#define C_BROWN  	0x6
#define C_LGREY  	0x7
#define C_DGREY  	0x8
#define C_LBLUE 	0x9
#define C_LGREEN  	0xA
#define C_LCYAN  	0xB
#define C_LRED  	0xC
#define C_LMAGENTA  0xD
#define C_LBROWN  	0xE
#define C_WHITE  	0xF

typedef unsigned char color_t;

color_t palette(color_t,color_t);

color_t palette(color_t fg, color_t bg)
{
	bg = bg << 0x4;	//Shift 4 bits to the left
	return (fg | bg);	//Return newly mixed color
}

color_t invert(color_t color)
{
	color_t a = (color & 0x0f); //bitwise AND with 00001111 to get lower bits
	color_t b = (color & 0xf0); //bitwise AND with 11110000 to get upper bits
	return palette(b,a);
}
