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


static const char spin_chars[] = { '|', '/', '-', '\\', };
static int spin_state=0;
static bool waiting=false;
static int loc;
static char ch;

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








