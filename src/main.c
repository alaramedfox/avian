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
extern void __cursor_hide(void);

/* Low-level headers and utilities */
#include "string.h"
#include "memorymap.h"	//Memory mapping and handling
#include "math.h"			//Math processing and casting
#include "stdio.h"		//Keyboard input and some string processing
#include "keyboard.h"	//Keyboard definition and init

/* Mid-level processing */
#include "lxtem.h"		//Core command library

void boot(void) 
{
	clear();
	setcolor(C_BLUESCR);
	move(0,0);
	for(int i=0;i<80;i++) 	{ print("&h1"); }
	for(int i=0;i<80*4;i++) { print(" "); }
	for(int i=0;i<80;i++) 	{ print("&h1"); }
	move(1,3); print("Welcome to the Popcorn Kernel v1.2");
	move(2,5); print("Initializing IRQ..."); 			idt_init();
	move(3,5); print("Initializing keyboard driver..."); 	kb_init();
	move(4,5); print("Using the LXTEM command enviornment");
	setcolor(C_TERMINAL);
}

void kmain(void) 
{
	memorymap_init();
	boot();
	//MEMORY.FLAGS.debugmode = true;
	move(6,0);
	while(1) {
		//newline();
		print("\n> ");
		process_raw_input(scan());
	}
	return;
}
