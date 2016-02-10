/* 
 *	Popcorn Kernel
 *	  File:		kernel.c
 *	  Purpose:	Main loop, definitions, and primary entry point.
 *				This is the Popcorn start file.
 */
 
/* 
 *		Popcorn Source Naming Conventions
 *
 *		1. Memory structures must be in all caps and start with two underscores.
 *				eg. __STRUCTURE
 *		2. ASM function calls must start with two underscores
 *				eg. __write_port
 *		3. All functions and variables are lowercase, and separate words using
 *			underscores.
 *				eg. some_variable, some_function()
 *		4. Loop indexes are i, j, and k. Avoid anything deeper than O(3)
 *		5. Related groups of functions are prefixed namespace-like.
 *				eg. all stdin functions are stdin_foo()
 *		6. Shell code is in directory /lex/
 *		7. Drivers are in directory /dev/
 *
 */

#include <stdint.h>		//Stable integer sizing. int8_t, int16_t, etc
#include <stddef.h>	
#include <stdbool.h>		//Boolean support
#include "icxxabi.c"		//Global constructors

/* System core */
#include "types.h"
#include "util.h"
#include "memory.h"

/* Low-level headers and utilities */
#include "string.h"		//Constant string class
#include "sstream.h"		//Stack-like string streams
#include "envar.h"		//Memory mapping and handling
#include "math.h"			//Math processing and casting
#include "stdio.h"		//Keyboard input and some char* processing
#include "keyboard.h"	//Keyboard definition and init

/* Mid-level processing */
//#include "lex/lxtem.h"		//Core command library

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
	stdout.print("\tPopcorn Kernel v1.5\n");
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
