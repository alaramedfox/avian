/*
*  kernel.c
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

#include "stdio.h"
#include "vbuffer.h"
#include "keyboard.h"

void boot(void)
{
	clear();
	move(0,0);
	print("Booting POPCORN..."); newline();
	print("  Initializing IRQ..."); newline();
	idt_init();
	print("  Initializing keyboard driver..."); newline();
	kb_init();
}

void kmain(void)
{
	boot();
	move(4,0);
	//stdin_clear();
	while(1)
	{
		move(4,0);
		print("> ");
		print(std_stream); //Command echo
		print("_ ");
		//if(STDIN_FLAG == 1) { print("_"); STDIN_FLAG = 0; }
		
	}
	
	return;
}
