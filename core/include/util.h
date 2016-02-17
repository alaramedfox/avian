#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
/*
 *		Avian Project
 *			File:		util.h
 *			Purpose:	Provide essential core functions
 */
 
#include <types.h>

/* Function macros */
#define until(exp) 	while(!(exp)) 	//Perl-like until loop
#define unless(exp)	if(!(exp))   	//Perl-like unless switch
#define STR(x)			#x
#define QW(x)			"x"

enum __BASE
{
	DEC=10, HEX=16, OCT=8, BIN=2,
};

size_t strlen(const char*);

char* itoa(int,int);
char* itoa_bytes(int32_t);
 
#endif