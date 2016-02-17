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



/* Verbose integer types */
typedef uint16_t index_t;
typedef uint16_t port_t;

/* Status type, for function return values */
enum __STATUS_CODES { S_OKAY, S_WARN, S_FAIL, };

typedef struct __STATUS
{
	int8_t	level;
	char*		message;
	
} status_t;

status_t OKAY(void);
status_t FAIL(char*);
status_t WARN(char*);

/* Types for special types of pointers */
typedef uintptr_t addr_t;	//Integer-casted pointer address

/* Types for constant sizes */
typedef uint8_t	byte;		//8-bit value		1 byte
typedef uint16_t	word;		//16-bit value		2 bytes
typedef uint32_t	dword; 	//32-bit value		4 bytes
 
#endif