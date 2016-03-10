#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/time.h
//      Purpose:         Handle the PIT and time-related functions
// ======================================================================== */

#include <stdlib.h>

void      time_init(word);

uint32_t clock(void);
void      sleep(uint32_t);   //Sleep for N ms
void      pit_handler(void);

#endif