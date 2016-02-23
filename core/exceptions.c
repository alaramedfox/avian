#define EXCEPTIONS_C_SOURCE
#include <exceptions.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/exceptions.c
 *		Purpose:	Exception handling
 */
#include <vga.h>
#include <stdio.h>
#include <color.h>
#include <buildcount.h>

static byte error_level=0;

static void panic_screen(void)
{
	vga_setcolor(C_BLUESCR);	//Famous Windows bluescreen
	vga_clear();
	
	
	/* Print kernel information */
	print("\n");
	print("\tAVIAN Kernel - " TIMESTAMP "\n\n");
	print("\tAn unhandled exception has occured,\n"
			"\tand execution was unable to continue.\n");
}

void catch_zero_divide(void)
{
	panic_screen();
	print("\tFault: Divide by Zero error\n");
	while(true);
}

