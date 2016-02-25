#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
/*
 *		Avian Project
 *			File:		util.h
 *			Purpose:	Provide essential core functions
 */
#include <types.h>
#include <time.h>

#define bitmask(var,bit)	var & ~(1 << bit)
#define bitset(var,bit)		var | (1 << bit)
#define bitcheck(var,bit)	var & (1 << bit)

#define BENCHMARK(time, function) 	\
		time = clock();					\
		function;							\
		time = (clock() - time);

typedef enum __BASE
{
	DEC=10, HEX=16, OCT=8, BIN=2, BYTES=-1, BOOLEAN=0,
	
} base_t;

void ASSERT(const char[], int, int, base_t);
void print_time(void);
void wait_spin(bool);

char* itoa(int,base_t);
char* itoa_bytes(int32_t);
 
#endif