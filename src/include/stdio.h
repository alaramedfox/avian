#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/include/stdio.h
//      Purpose:        C Standard Library - Input / Output
// ======================================================================== */

#include <stdlib.h>
#include <stdarg.h>

#define BUFSIZE 80

typedef struct __FILE_STREAM
{
   char* name;
   void* data;
   
} FILE;

enum __COLOR_DEFS
{
   BLACK = 0x0, BLUE  = 0x1, GREEN = 0x2, CYAN  = 0x3,
   RED   = 0x4, MAG   = 0x5, BROWN = 0x6, GREY  = 0x7,
   
   C_TERM  = 0x07, C_BLUESCR=0x1F, 
   C_WARN  = 0x04, C_CRIT = 0x47,
};

typedef uint32_t fpos_t;
typedef uint32_t off_t;

int scan(char* buffer);

int printf(const char* format, ...);
int scanf(const char* format, ...);


#endif