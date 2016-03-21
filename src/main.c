#define MAIN_C_SOURCE
/*  
 *      Avian Kernel - Bryan Webb
 *        File:      main.c
 *        Purpose:   Main loop, definitions, and primary entry point.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <lex.h>
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
   //types_test();
   //colors_test();
   exceptions_init();
   hide_cursor();
   vga_setcolor(0x07);
   pic_init();
   ENVAR_init();      //Init global values
   kb_init();         //Init keyboard driver
   floppy_init();
   
   printf("\nSystem booted.\n\n");
}

void avian_main(void) 
{  
   bootscreen();
   init();
   
   //printf("AnicaFS: Allocation of Nodes by Indexed Cluster Addresses\n");
   
   //anica_format_device(2880, 512, 1);
/*
   volume_t* floppy = mount(fda);
   file_t* file = open(floppy, "TEST.TXT", ANICA_WRITE);
   
   read(file);
   printf("Contents of file: `%s'\n",file->data);
   
   print("Enter the new content: "); 
   
   char* newcontent = (char*) malloc(128);
   scan(newcontent, 128);
   chomp(newcontent);
   print("\n");
   
   write(file, newcontent);
   free(newcontent);
   
   read(file);
   printf("Contents of file: `%s'\n",file->data);
    
   unmount(floppy);
*/  
   printf("Entering shell\n");
   
   //while(main_loop());
   shell();
   
   
   vga_clear();
   printf("Kernel has shut down\n");
}

bool main_loop(void)
{  
   //sleep(1000);
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
   printf("[ Avian Kernel version " VERSION " ]\n\n");
   
   vga_setcolor(0x07);
}


