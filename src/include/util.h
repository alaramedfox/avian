#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/util.h
//      Purpose:         Provides misc functions and types
// ======================================================================== */

#include <stdlib.h>
#define this __func__
#define ASSERT(expr) trace_function(this); expr

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

void wait_spin(bool);
void types_test(void);
void colors_test(void);

void trace_function(const char function[]);
void notify_progress(const char[], int, int);
 
#endif