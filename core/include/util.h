#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
/*
 *		Avian Project
 *			File:		util.h
 *			Purpose:	Provide essential core functions
 */
#include <types.h>
#include <time.h>

/* Function macros */
#define until(exp) 	while(!(exp)) 	//Perl-like until loop
#define unless(exp)	if(!(exp))   	//Perl-like unless switch
#define STR(x)			itoa(x,10)
#define QW(x)			#x

#define bitmask(var,bit)	var & ~(1 << bit)
#define bitset(var,bit)		var | (1 << bit)
#define bitcheck(var,bit)	var & (1 << bit)


typedef enum __BASE
{
	DEC=10, HEX=16, OCT=8, BIN=2, BYTES=-1, BOOLEAN=0,
	
} base_t;

void ASSERT(const char[], int, int, base_t);
void print_time(void);
void wait_spin(bool);

size_t strlen(const char*);

char* itoa(int,base_t);
char* itoa_bytes(int32_t);
 
#endif