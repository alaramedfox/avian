#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/time.h
//      Purpose:         Handle the PIT and time-related functions
// ======================================================================== */

#define timeout_ms(var, time, expr) \
   var = 0;                         \
   for(int i=0; i<time; i++) {      \
      sleep(1);                     \
      expr                          \
   }                                

void sleep(int);
void usleep(int);
short get_bios_ticks(void);

#endif