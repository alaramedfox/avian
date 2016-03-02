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

void avian_main(void) 
{
	bootscreen();
	init();
	
	print("Testing accuracy of floppy controller\n");
	int time = clock();
//#define TEST_IO

#if defined TEST_IO	
	byte* test = (byte*) malloc(512);
	for(int i=0; i<512; i++) test[i] = 'T';
	
	
	floppy_write_block(37, test, 1);
	
	
	for(int i=0; i<64; i++) {
		print(itoa(test[i],HEX));
		print(" ");
	}
	
#else
	print("Mounting volume\n");
	volume_t* floppy = mount(fda);
	
	print("Opening file\n");
	file_t* file = open(floppy, "AT.TXT", DFS_WRITE);
	
	char *source_string = (char*) malloc(512);
	for(int i=0; i<512; i++) source_string[i] = '@';
	
	char *test_string = "If this shows up, there is an overflow";
	
	char* result_string = (char*) malloc(128);
	
	print("Writing file\n");
	write(file, source_string, 512);
	
	print("Reading file\n");
	read(file, result_string, 128);
	
	print("Contents of file: `"); print(result_string); print("'\n");
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


