#define MAIN_C_SOURCE
/* 
 *      Avian Kernel - Bryan Webb
 *        File:      main.c
 *        Purpose:   Main loop, definitions, and primary entry point.
 */

#include <stdlib.h>
#include <string.h>

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
   time_init(1000);   //Init clock to measure milliseconds
   ENVAR_init();      //Init global values
   kb_init();         //Init keyboard driver
   floppy_init();
   ENVAR.FLAGS.listen = false;
   
   print("\nSystem booted.\n\n");
}

void avian_main(void) 
{
   bootscreen();
   init();

   print("AnicaFS: Allocation of Nodes by Indexed Cluster Addresses\n");
   int time = clock();
   
   anica_format_device(2880, 512, 1);
   
   volume_t* floppy = mount(fda);
   file_t* file = open(floppy, "TEST.TXT", ANICA_WRITE);
   
   read(file);
   print("Contents of file: `"); print(file->data); print("'\n");
   
   char* newcontent = new_str("This is the new file content");
   write(file, newcontent);
   free(newcontent);
   
   read(file);
   print("Contents of file: `"); print(file->data); print("'\n");
   
   unmount(floppy);
   
   print("\nTest complete after "); iprint(clock()-time,DEC); print("ms\n");

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
   for(size_t i=0; i<80; i++)    { addch(HLINE1); }
   vga_moveptr(location+4);
   print("[ Avian Kernel version " VERSION " ]\n\n");
   
   vga_setcolor(0x07);
}


