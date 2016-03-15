#define UTIL_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/util.c
//      Purpose:         Provide misc functions
// ======================================================================== */

#include <util.h>
#include <stdlib.h>
#include <string.h>

#include <vga.h>
#include <time.h>

#define psizeof(T)            \
   vga_setcolor(0x02);        \
   print(#T);                 \
   vga_setcolor(0x07);        \
   print("\t(");              \
   iprint(sizeof(T)*8,DEC);   \
   print(")\t")

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
   print("[ ");            
   vga_setcolor(0x05);     
   print(function);        
   vga_setcolor(0x07);     
   print(" ] ");          
}

void print_time(void)
{
   print("[ ");
   vga_setcolor(0x02);
   iprint(clock(),DEC);
   vga_setcolor(0x07);
   print(" ] ");
}

void notify(const char str[])
{
   print_time();
   print(str);
}

void notify_progress(const char str[], int current, int target)
{
   print(str);
   print(" ("); iprint(current,DEC); print("/");
   iprint(target,DEC); print(")");
   if(current >= target) print("\n");
   else print("\r");
}








