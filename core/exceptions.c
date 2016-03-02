#define EXCEPTIONS_C_SOURCE
#include <exceptions.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/exceptions.c
 *		Purpose:	Exception handling
 */
#include <vga.h>
#include <color.h>
#include <buildcount.h>
#include <idt.h>
#include <util.h>

static byte error_level=0;

#define FAULT(str)	print("\n\tError: "); print(str);
#define VALUE(val)	print("\n\tStack: "); print(itoa(val,HEX));

void exceptions_init(void)
{
	for(int i=0; i<13; i++) {
		idt_add_exception((addr_t)throw_exception, i);
	}
	
	idt_add_exception((addr_t)throw_zero_divide, X_ZERO_DIVIDE);
	idt_add_exception((addr_t)throw_double_fault, X_DOUBLE_FAULT);
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

void catch_double_fault(int arg)
{
	panic_screen();
	FAULT("Double Fault");
	VALUE(arg);
	while(true);
}

void catch_zero_divide(void)
{
	//panic_screen();
	FAULT("Divide by zero");
	while(true);
}

