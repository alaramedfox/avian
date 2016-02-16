#define COLOR_C_SOURCE
#include <color.h>
/*
 *		Avian Project
 *		File:			/lib/color.c
 *		Purpose:		Manipulate color bitfields for VGA
 */
 


color_t color_mix(int8_t fg, int8_t bg)
{
	return ( (bg << 4) | fg);
}

color_t color_bg(color_t c)
{
	return ( (c & 0xF0) >> 4 );
}

color_t color_fg(color_t c)
{
	return (c & 0x0F);
}