#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED
#include <types.h>

/*	
 *		Avian Project - Bryan Webb
 *		File:		/include/color.h
 *		Purpose:	Header file for color.c 
 */
 
enum __COLOR_DEFS
{
	BLACK=0x0, BLUE=0x1, GREEN=0x2, CYAN=0x3,
	RED=0x4, MAGENTA=0x5, BROWN=0x6, GREY=0x7,
	BOLD=1, NORMAL=0,
	
	C_TERMINAL=0x07, C_BLUESCR=0x1F, C_ERR=0x04,
};
 

typedef byte color_t;

color_t color_mix(int8_t, int8_t);
color_t color_bg(color_t);
color_t color_fg(color_t);


#endif