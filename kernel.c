/* 
 *	Popcorn Kernel
 *	  File:		kernel.c
 *	  Purpose:	Main loop, definitions, and primary entry point.
 *				This is literally the most important file.
 */

void pause(int x)
{
	int l=4;
	for(int i=x; i>0; i--)
	{
		for(int j=x; j>0; j--)
		{
			volatile int k = j;
			l += k;
		}
	}
}

/* GLOBAL DEFINITIONS */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "math.h"

#include "vbuffer.h"
#include "stdio.h"
#include "keyboard.h"

void boot(void)
{
	clear();
	setcolor_pair(C_WHITE,C_BLUE);
	move(0,0);
	for(int i=0;i<80;i++) { print("\\-"); }
	for(int i=0;i<80*3;i++) { print(" "); }
	for(int i=0;i<80;i++) { print("\\-"); }
	move(1,3);
	print("Welcome to the Popcorn Kernel v1.0"); newline();
	move(2,5); print("Initializing IRQ...");
	idt_init();
	move(3,5); print("Initializing keyboard driver...");
	setcolor_pair(C_WHITE,C_BLACK);
	kb_init();
}

void kmain(void)
{
	boot();
	move(5,0);
	//stdin_clear();
	while(1)
	{
		stdin(ECHO);
		newline(); 
		
	}
	
	return;
}
