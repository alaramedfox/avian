/* 
 *	Popcorn Kernel
 *	  File:		kernel.c
 *	  Purpose:	Main loop, definitions, and primary entry point.
 *				This is the Popcorn start file.
 */

#include <stdint.h>		//Stable integer sizing. int8_t, int16_t, etc
#include <stddef.h>	
#include <stdbool.h>		//Boolean support

/* Low-level headers and utilities */
#include "memorymap.h"	//Memory mapping and handling
#include "math.h"			//Math processing and casting
#include "stdio.h"		//Keyboard input and some string processing
#include "keyboard.h"	//Keyboard definition and init

/* Mid-level processing */
#include "command.h"		//Core command library

void boot(void) 
{
	clear();
	setcolor(C_BLUESCR);
	move(0,0);
	for(int i=0;i<80;i++) 	{ print("&h1"); }
	for(int i=0;i<80*4;i++) { print(" "); }
	for(int i=0;i<80;i++) 	{ print("&h1"); }
	move(1,3); print("Welcome to the Popcorn Kernel v1.1");
	move(2,5); print("Initializing IRQ..."); 			idt_init();
	move(3,5); print("Initializing keyboard driver..."); 	kb_init();
	move(4,5); print("Using the LXTCM command enviornment");
	setcolor(C_TERMINAL);
}

void kmain(void) 
{
	memorymap_init();
	boot();
	
	move(0,0); print("X");
	move(6,0);
	while(1) {
		//newline();
		print("\n> ");
		process_raw_input(scan());
	}
	return;
}
