#define FILE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/errors.c                                    
//    Purpose:       Error flag implementation                    
// ======================================================================== */

#include <errors.h>
#include <vga.h>

static void print_badge(int severity)
{
   print("[ ");
   switch(severity)
   {
      case 0: vga_setcolor(0x02); print("NOTE"); break;
      case 1: vga_setcolor(0x06); print("WARN"); break;
      case 2: vga_setcolor(0x04); print("FAIL"); break;
   }
   vga_setcolor(0x07);
   print(" ] ");
}

void throw(const char message[], int severity)
{
   if(vga_getcol() != 0) print("\n");
   print_badge(severity);
   println(message);
}