/* 
 *	Popcorn Kernel
 *	  File:		kernel.c
 *	  Purpose:	Main loop, definitions, and primary entry point.
 *				This is literally the most important file.
 */

/* GLOBAL DEFINITIONS */

#include <stdint.h>		//Stable integer sizing. int8_t, int16_t, etc
#include <stddef.h>	
#include <stdbool.h>	//Boolean support

/* Low-level headers and utilities */
#include "math.h"		//Math processing and casting
#include "vram.h"		//Video output and handling
#include "stdio.h"		//Keyboard input and some string processing
#include "keyboard.h"	//Keyboard definition and init

/* Mid-level processing */
#include "command.h"	//Core command library

void boot(void)
{
	clear();
	setcolor_pair(C_WHITE,C_BLUE);
	move(0,0);
	for(int i=0;i<80;i++) 	{ print("&h1"); }
	for(int i=0;i<80*3;i++) 	{ print(" "); }
	for(int i=0;i<80;i++) 	{ print("&h1"); }
	move(1,3); print("Welcome to the Popcorn Kernel v1.0");
	move(2,5); print("Initializing IRQ..."); 			idt_init();
	move(3,5); print("Initializing keyboard driver..."); 	kb_init();
	
	setcolor_pair(C_WHITE,C_BLACK);
}

void kmain(void)
{
	boot();
	move(5,0);
	//stdin_clear();
	
	int test = 4108;
	while(1)
	{
		print("> ");
		char* input = stdin(ECHO);
		newline();
	}
	
	return;
}