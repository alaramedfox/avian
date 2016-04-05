#define MAIN_C_SOURCE
// ========================================================================= //
//    Avian Kernel   Bryan Webb (C) 2016
//    File:          main.c
//    Purpose:       Kernel entry point and testing ground
// ========================================================================= //

/* Include header warning */
#if defined FILE_H_INCLUDED
   #error You may have forgotten to rename the default header!
#endif 

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <lex.h>
#include <vga.h>
#include <idt.h>
#include <exceptions.h>
#include <pic.h>
#include <keyboard.h>
#include <util.h>
#include <buildcount.h>
#include <floppy.h>
#include <filesystem.h>
#include <anicafs.h>

void init(void);
void bootscreen(void);

void init(void) 
{
   exceptions_init();
   hide_cursor();
   vga_setcolor(0x07);
   pic_init();
   kb_init();         //Init keyboard driver
   floppy_init();
   lex_init();
   printf("\nSystem booted.\n\n");
}

void avian_main(void) 
{  
   bootscreen();
   init();
   
   printf("Entering shell\n");
   shell();
   
   vga_clear();
   printf("Kernel has shut down\n");
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


