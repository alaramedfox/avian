#define UTIL_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/util.c
//      Purpose:         Provide misc functions
// ======================================================================== */

#include <util.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <vga.h>

#define psizeof(T)   printf("%#%s%#\t(%i)\t",0x02,#T,0x07,sizeof(T)*8)

void types_test(void)
{
   vga_tabsize(10);
   print(" C Standard:\tC Library:\tFixed-width:\tCustom Avian:\n");
   print(" "); psizeof(char);  psizeof(size_t);    psizeof(int8_t);  psizeof(byte); print("\n");
   print(" "); psizeof(short); psizeof(ptrdiff_t); psizeof(int16_t); psizeof(word); print("\n");
   print(" "); psizeof(int);   psizeof(intptr_t);  psizeof(int32_t); psizeof(dword); print("\n");
   print(" "); psizeof(long);  psizeof(intmax_t);  psizeof(int64_t); psizeof(addr_t); print("\n");
   print("\n");
   vga_tabsize(5);
}

void trace_function(const char function[])
{
   printf("[ %#%s%# ]",MAGENTA,function,GREY);
}

void notify_progress(const char str[], int current, int target)
{
   printf("%s (%i/%i)%c", str, current, target, current==target?'\n':'\r');
}








