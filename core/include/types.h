#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
/*
 *		Avian Project - Bryan Webb
 *		File:			/include/types.h
 *		Purpose:		Define a number of custom types
 *
 *		Note: The aliases defined below are purely for vanity.
 *				I personally don't like using types such as 'uintptr_t',
 *				because they are ugly and mildly difficult to interpret.
 *				Therefore, I have aliased them into (slightly) more intuitive
 *				and (vain) names.
 */

/* Standard C Library types */
#include <stdint.h>		//For int16_t, etc
#include <stdbool.h>		//Booleans, true/false
#include <stddef.h>		//size_t

enum __STATUS_LEVEL
{
	OK, WARNING, ERROR, SEVERE, 
};

/* Types for special types of pointers */
typedef uintptr_t addr_t;	//Integer-casted pointer address

/* Status type, for function return values */
typedef struct __STATUS
{
	int8_t	level;
	char*		message;
	
} status_t;

status_t STATUS(int8_t, char*);
status_t STATUS_OK(void);

/* Types for constant sizes */
typedef uint8_t	byte;		//8-bit value
typedef uint16_t	word;		//16-bit value
typedef uint32_t	dword; 	//32-bit value
 
#endif