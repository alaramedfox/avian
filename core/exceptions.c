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
#include <idt.h>

static byte error_level=0;

void exceptions_init(void)
{
	for(int i=0; i<13; i++) {
		idt_add_exception((addr_t)throw_exception, i);
	}
}

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

void catch_exception(void)
{
	panic_screen();
	while(true);
}

void catch_zero_divide(void)
{
	panic_screen();
	print("\tFault: Divide by Zero error\n");
	while(true);
}

