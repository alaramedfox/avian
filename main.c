#define MAIN_C_SOURCE
/* 
 *		Avian Kernel - Bryan Webb
 *	  	File:		main.c
 *	  	Purpose:	Main loop, definitions, and primary entry point.
 */
 
#define DEBUGMODE

#include <stdlib.h>
#include <string.h>
#include <types.h>
#include <vga.h>
#include <envar.h>
#include <idt.h>
#include <exceptions.h>
#include <pic.h>
#include <keyboard.h>
#include <util.h>
#include <buildcount.h>
#include <floppy.h>
#include <time.h>
#include <filesystem.h>
#include <lindafs.h>

void init(void);
bool main_loop(void);
void bootscreen(void);

void init(void) 
{
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

/* Working offsets

   0x4806   18438    36
   0x9006   36870    72
   0xd806   55302

*/

void avian_main(void) 
{
	bootscreen();
	init();
//#define LINDA
	print("Testing accuracy of floppy controller\n");
	int time = clock();
#define LINDA
#if defined LINDA
	print("Formatting device to Linda\n");
	linda_format_device(2880, 512, 0, 2);
#else
   byte* block = (byte*) malloc(512);
   foreach(i, 512) block[i] = 0xFF;
   memcpy(block, "Sector #", 8);
   
   foreach(i, 80) {
      notify("Writing sector "); print(itoa(i,DEC)); print("\n");
      memcpy(block+8, itoa(i,DEC), strlen(itoa(i,DEC)));
      if(!floppy_write_block(i, block, 512)) {
         print("IO Error\n");
      }
      //sleep(1000);
   }
   
   free(block);
#endif
	print("\nTest complete after "); print(itoa(clock()-time,DEC)); print("ms\n");

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


