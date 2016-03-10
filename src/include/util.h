#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/util.h
//      Purpose:         Provides misc functions and types
// ======================================================================== */

#include <stdlib.h>

#define ASSERT(expr)       \
   print("[ ");            \
   vga_setcolor(0x05);     \
   print(__func__);        \
   vga_setcolor(0x07);     \
   print(" ] ");           \
   expr

typedef union __PAIR16
{
   byte _byte[2];
   word _word;

} pair16_t;

typedef union __PAIR32
{
   byte _byte[4];
   dword _dword;

} pair32_t;

void print_time(void);
void wait_spin(bool);

void notify(const char[]);
void notify_progress(const char[], int, int);
 
#endif