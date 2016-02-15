/* 
 *	Popcorn Kernel
 *	  File:		main.c
 *	  Purpose:	Main loop, definitions, and primary entry point.
 *				This is the Popcorn start file.
 */

#include <types.h>
#include <vga.h>
#include <envar.h>
#include <idt.h>
#include <keyboard.h>
#include <stdio.h>

void init(void) 
{
	ENVAR_init();		//Init global values
	idt_init();			//Init interrupt controller
	kb_init();			//Init keyboard driver
	ENVAR.FLAGS.listen = false;
}

void bootscreen(void)
{
	vga_setcolor(0x07);
	vga_clear();
	
	vga_setcolor(0x1F);
	for(size_t i=0; i<80; i++) 	{ addch(HLINE1); }
	for(size_t i=0; i<80*4; i++) 	{ addch(' '); }
	for(size_t i=0; i<80; i++) 	{ addch(HLINE1); }
	
	vga_movexy(1,0);
	print("\tAvian Kernel version 0.6.1\n");
	print("\tGNU Public Liscense -- Bryan Webb, 2016\n");
	vga_setcolor(0x07);
}

extern void C_main(void) 
{
	init();
	bootscreen();
	vga_movexy(5,0);
	
	while(1) {
		print("\n> ");
		scan();
	}
}
