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

static void __panic_screen(void)
{
	vga_clear();
	vga_setcolor(C_BLUESCR);	//Famous Windows bluescreen
	
	/* Print kernel information */
	print("AVIAN Kernel - " TIMESTAMP "\n");
	print("An unhandled exception has occured, "
			"and execution was unable to continue.\n");
}

void catch_zero_divide(void)
{
	__panic_screen();
	print("\tFault: Divide by Zero error\n");
	while(true);
}