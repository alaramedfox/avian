#define MAIN_C_SOURCE
/* 
 *		Popcorn Kernel
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

void init(void) 
{
	ENVAR_init();		//Init global values
	idt_init();			//Init interrupt controller
	kb_init();			//Init keyboard driver
	ENVAR.FLAGS.listen = false;
}

void main_loop(void)
{
	// garbage collection
	
	
	/* Check for and handle errors */
	switch(ENVAR.GLOBAL.status.level)
	{
		case S_WARN: 
			vga_setcolor(C_WARN);
			print("\n[ WARNING ] ");
			print(ENVAR.GLOBAL.status.message);
			print("\n");
			vga_setcolor(0x07);
			ENVAR.GLOBAL.status = OKAY();
			break;
			
		case S_FAIL: 
			vga_setcolor(C_CRITICAL);
			print("\n[ ERROR ] ");
			print(ENVAR.GLOBAL.status.message);
			print("\n");
			vga_setcolor(0x07);
			while(true);
			
			break;
			
		default: break;
	}
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
	
	while(1) {
		char* boo = (char*) malloc(1024);
		main_loop();
	}
}
