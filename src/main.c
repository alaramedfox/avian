/* 
 *	Popcorn Kernel
 *	  File:		main.c
 *	  Purpose:	Main loop, definitions, and primary entry point.
 *				This is the Popcorn start file.
 */
 
/* 
 *		Popcorn Source Naming Conventions
 *
 *		1. 	Memory structures must be in all caps and start with two underscores.
 *					eg. __STRUCTURE
 *		2. 	ASM function calls must start with two underscores
 *					eg. __write_port
 *		3. 	All functions and variables are lowercase, and separate words using
 *				underscores.
 *					eg. some_variable, some_function()
 *		4. 	Loop indexes are i, j, and k. Avoid anything deeper than O(3)
 *		4.1 	Loops that use a variable as a counter, use type count_t
 *		4.2	Loops that use a variable as an index, use type index_t
 *
 */
 
#define VERSION "0.5.2"

#include <stdint.h>		//Stable integer sizing. int8_t, int16_t, etc
#include <stddef.h>	
#include <stdbool.h>		//Boolean support

/* Essential headers */
#include "defines.h"
#include "core.h"
#include "stdlib.h"
#include "devices.h"

void init(void) 
{
	ENVAR.init();		//Init global values
	stdin = CIN();		//Init standard input stream
	stdout = VGA();	//Init standard output stream (screen)
	idt_init();			//Init interrupt controller
	kb_init();			//Init keyboard driver
	stdout.clear();	//Fill screen with blank spaces
}

void bootscreen(void)
{
	stdout.setcolor(C_BLUESCR);
	for(count_t i=0; i<C.cols; i++) { stdout.addch(HLINE1); }
	for(count_t i=0; i<C.cols*4; i++) { stdout.addch(' '); }
	for(count_t i=0; i<C.cols; i++) { stdout.addch(HLINE1); }
	
	stdout.move(1,0);
	stdout.print("\tPopcorn Kernel v"); stdout.print(VERSION); stdout.print("\n");
	stdout.print("\tGNU Public Liscense -- Bryan Webb, 2016\n");
	
	stdout.setcolor(C_TERMINAL);
}

extern "C" void main(void) 
{
	init();
	bootscreen();
	
	stdout.move(6,0);

	while(1) {
		stdout.print("\n> ");
		string input = stdin.scan();
		stdout.print("(");
		stdout.print(input);
		stdout.print(")");
	}
}
