#define COLOR_C_SOURCE
/* ======================================================================= */
/*		Avian Kernel   Bryan Webb (C) 2016
/*		File:			   /core/color.c
/*		Purpose:		   Manipulate color bitfields for VGA
/* ======================================================================= */
 
#include <color.h>

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