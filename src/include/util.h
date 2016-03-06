#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
// ======================================================================== */
//		Avian Kernel   Bryan Webb (C) 2016
//		File:		      avian/include/util.h
//		Purpose:	      Provides misc functions and types
// ======================================================================== */

#include <stdlib.h>

typedef enum __BASE
{
	DEC=10, HEX=16, OCT=8, BIN=2, BYTES=-1, BOOLEAN=0,
	
} base_t;

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

char* itoa(int,base_t);
char* itoa_bytes(int32_t);
 
#endif