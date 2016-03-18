#define FILE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/errors.c                                    
//    Purpose:       Error flag implementation                    
// ======================================================================== */

#include <errors.h>
#include <vga.h>

void throw(const char function[], int error)
{
   if(vga_getcol() != 0) print("\n");
   vga_setcolor(C_WARN);
   print("Error: ");
   vga_setcolor(0x07);
   print("in function `"); print(function); print("' with status ");
   iprint(error,HEX);
   print("\n");
}