#define MAIN_C_SOURCE
#define __
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
#include <util.h>

void init(void) 
{
	ENVAR_init();		//Init global values
	idt_init();			//Init interrupt controller
	kb_init();			//Init keyboard driver
	ENVAR.FLAGS.listen = false;
}

status_t main_loop(void)
{
	
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
	print("[ Avian Kernel version 0.6.3 ]");
	
	
	vga_setcolor(0x07);
}


extern void C_main(void) 
{
	init();
	bootscreen();
	char* ub;
	vga_setcolor(0x1F);
	vga_movexy(1,4); print("Maximum blocks: "); print(itoa(MAX_BLOCKS,10));
	vga_movexy(2,4); print("Current blocks: "); 
	vga_movexy(3,4); print("Used Memory:    ");
	
	while(1) {
		int32_t *giant = (int32_t*) malloc(1024);
		vga_movexy(2,20); print(itoa(mem_blocks(),10));
		vga_movexy(3,20); print(itoa(mem_used(),10));
	}
}
