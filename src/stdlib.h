#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
/* 
 *	Popcorn Kernel
 *	  File:		stdlib.h
 *	  Purpose:	Header for standard functional library
 */
 
/* Utilities */
#include "lib/math.c"

extern "C" size_t strlen(const char*);
#include "lib/util.c"

/* Libraries */
extern "C" class string;
extern "C" class stringstream;
#include "lib/string.c"
#include "lib/sstream.c"


/* Input and output streams */
extern "C" class VGA;
extern "C" class CIN;
#include "lib/vga.c"
#include "lib/cin.c"

#endif
