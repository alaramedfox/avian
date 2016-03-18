#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/include/stdio.h
//      Purpose:        C Standard Library - Input / Output
// ======================================================================== */

#include <stdlib.h>

typedef uint32_t fpos_t;

int printf(const char* format, ... );

int scan(char* buffer, size_t len);

#endif