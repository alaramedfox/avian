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
	int offset = 0;
	move(4,0);
	
	unsigned int last_vptr=0;
	
	while(1)
	{
		
	}
	
	return;
}
