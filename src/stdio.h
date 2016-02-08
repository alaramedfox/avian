/* 
 *	Popcorn Kernel
 *	  File:		stdio.h
 *	  Purpose:	File and keyboard input and output handling
 */
 
#define STD_MAX	256			//Maximum characters for input 
#define ENTER_KEY_CODE 0x1C

#include "vga.c"
#include "cin.c"

#include "colordef.h"						//Color #DEFINEs and functions
#include "escapes.h"


namespace iostream 
{
	__STDOUT stdout;
	__STDIN stdin;
	
}


