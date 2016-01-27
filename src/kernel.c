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
		print("Integer test: ");
		print(itos(-889));
		newline();
	}
	
	return;
}
