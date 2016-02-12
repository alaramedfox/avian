#define COLOR_C_SOURCE
#include <color.h>
/*
 *		Popcorn Kernel
 *		File:			/lib/color.c
 *		Purpose:		Manipulate color bitfields for VGA
 */
 
enum __COLOR_DEFS
{
	BLACK=0x0, BLUE=0x1, GREEN=0x2, CYAN=0x3,
	RED=0x4, MAGENTA=0x5, BROWN=0x6, GREY=0x7,
	BOLD=1, NORMAL=0,
};


typedef struct __COLOR_T 
{
	int8_t bg : 4;
	int8_t fg : 4;
	
} color_t;

color_t color_mix(int8_t fg, int8_t bg)
{
	color_t color;
	color.fg = fg;
	color.bg = bg;
	return color;
}

color_t color_inv(color_t color)
{
	return color_mix(color.bg, color.fg);
}