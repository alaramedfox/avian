#define MAIN_C_SOURCE
/* 
 *		Avian Kernel - Bryan Webb
 *	  	File:		main.c
 *	  	Purpose:	Main loop, definitions, and primary entry point.
 */

#include <types.h>
#include <vga.h>
#include <envar.h>
#include <idt.h>
#include <keyboard.h>
#include <stdio.h>
#include <util.h>
#include <asmfunc.h>
#include <buildcount.h>
#include <floppy.h>



void init(void) 
{
	ENVAR_init();		//Init global values
	idt_init();			//Init interrupt controller
	kb_init();			//Init keyboard driver
	floppy_init();
	ENVAR.FLAGS.listen = false;
}

bool main_loop(void)
{
	// Test things
	
	// garbage collection
	
	/* Check for and handle errors */
	return true;
}

void bootscreen(void)
{
	vga_setcolor(0x07);
	vga_clear();
	vga_movexy(0,0);
	vga_setcolor(0x1F);
	for(size_t i=0; i<80; i++) 	{ addch(HLINE1); }
	for(size_t i=0; i<80*4; i++) 	{ addch(' '); }
	for(size_t i=0; i<80; i++) 	{ addch(HLINE1); }
	
	vga_movexy(0,4);
	print("[ Avian Kernel version " VERSION " ]");
	
	
	vga_setcolor(0x07);
}

extern void C_main(void) 
{
	init();
	bootscreen();
	
	while(main_loop());
	
	vga_clear();
	print("Kernel has shut down\n");
}
