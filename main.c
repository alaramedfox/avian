#define MAIN_C_SOURCE
/* 
 *		Avian Kernel - Bryan Webb
 *	  	File:		main.c
 *	  	Purpose:	Main loop, definitions, and primary entry point.
 */
 
#define DEBUGMODE

#include <stdlib.h>
#include <types.h>
#include <vga.h>
#include <envar.h>
#include <idt.h>
#include <keyboard.h>
#include <util.h>
#include <asmfunc.h>
#include <buildcount.h>
#include <floppy.h>
#include <stdio.h>
#include <time.h>
#include <fat.h>

void init(void);
bool main_loop(void);
void bootscreen(void);

void init(void) 
{
	ASSERT("Debug mode enabled",0,0,DEC);
	exceptions_init();
	hide_cursor();
	vga_setcolor(0x07);
	pic_init();
	time_init(1000);	//Init clock to measure milliseconds
	ENVAR_init();		//Init global values
	kb_init();			//Init keyboard driver
	floppy_init();
	ENVAR.FLAGS.listen = false;
	
	print("\nSystem booted.\n\n");
}



void avian_main(void) 
{
	bootscreen();
	init();
	
	print("Testing file input\n");
	int time = clock();
	
	byte *block = (byte*) malloc(512);
	file_t file;
	
	open(&file,DFS_READ,"FILE.TXT");
	
	print("Input complete after "); print(itoa(clock()-time,DEC)); print("ms\n");
	while(main_loop());
	
	
	vga_clear();
	print("Kernel has shut down\n");
	
}

bool main_loop(void)
{
	return true;
}

void bootscreen(void)
{
	vga_setcolor(0x07);
	vga_clear();
	vga_setcolor(C_BLUESCR);

	int location = vga_getloc();
	for(size_t i=0; i<80; i++) 	{ addch(HLINE1); }
	vga_moveptr(location+4);
	print("[ Avian Kernel version " VERSION " ]\n\n");
	
	vga_setcolor(0x07);
}


