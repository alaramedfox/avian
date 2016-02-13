#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED
#include <types.h>

/*	
 *		Popcorn Kernel - Bryan Webb
 *		File:		/include/color.h
 *		Purpose:	Header file for color.c 
 */
 

typedef byte color_t;

color_t color_mix(int8_t, int8_t);
color_t color_bg(color_t);
color_t color_fg(color_t);


#endif