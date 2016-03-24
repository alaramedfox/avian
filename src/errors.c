#define ERRORS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/errors.c                                    
//    Purpose:       Error flag implementation                    
// ======================================================================== */

#include <errors.h>
#include <stdio.h>
#include <vga.h>

static void print_badge(int severity)
{
   switch(severity)
   {
      //case 0: vga_setcolor(0x02); print("NOTE"); break;
      case 0: printf("[%# NOTE %#] ",GREEN,GREY); break;
      case 1: printf("[%# WARN %#] ",BROWN,GREY); break;
      case 2: printf("[%# FAIL %#] ",RED,GREY); break;
   }
}

void throw(const char message[], int severity)
{
   if(vga_getcol() != 0) printf("\n");
   print_badge(severity);
   printf("%s\n",message);
}