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
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int count_t;
typedef unsigned char color_t;

/* Low-level headers and utilities */
#include "string.h"
#include "memorymap.h"	//Memory mapping and handling
#include "math.h"			//Math processing and casting
#include "stdio.h"		//Keyboard input and some char* processing
#include "keyboard.h"	//Keyboard definition and init

/* Mid-level processing */
#include "lxtem.h"		//Core command library

void boot(void) 
{
	MEMORY.init();
	idt_init();
	kb_init();
	stdout::clear();
	stdin::clear();
	stdout::setcolor(C_BLUESCR);
	
	//for(count_t i=0;i<80;i++) 	 { stdout::print(" "); }
	//for(count_t i=0;i<80*4;i++) { stdout::print(" "); }
	//for(count_t i=0;i<80;i++) 	 { stdout::print(" "); }
	stdout::move(1,3); stdout::print("Welcome to the Popcorn Kernel v1.3");
	stdout::move(2,5); stdout::print("Initializing IRQ...");
	stdout::move(3,5); stdout::print("Initializing keyboard driver...");
	stdout::move(4,5); stdout::print("Using the LXTEM command enviornment");
	stdout::setcolor(C_TERMINAL);
	
}

extern "C"	//cdecl export
void kmain(void) 
{
	boot();
	while(1) {
		//newline();
		stdout::print("\n> ");
		string input = stdin::scan();
		//stdout::print("\n: "); stdout::print(input);
		process_raw_input(input);
	}
}
