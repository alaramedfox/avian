#define TIME_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/time.c
//      Purpose:         Handle the PIT and time-related functions
// ======================================================================== */
 
#include <time.h>
#include <asmfunc.h>

void usleep(int mcs)
{
   for(int i=0; i<mcs; i++) {
      outportb(0x80, 0);
      inportb(0x80);
   }
}

void sleep(int ms)
{
   usleep(ms*2000);
}