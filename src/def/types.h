#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		types.h
 *			Purpose:	define a number of custom types
 */

/* Types for special uses of int */
typedef unsigned int count_t;	//For loop counters
typedef unsigned int index_t;	//Memory indexes

/* Types for special uses of 8-bit bytes */
typedef unsigned char color_t;

/* Types for special types of pointers */
typedef uintptr_t address_t;	//Integer-casted pointer address

/* Types for constant sizes */
typedef uint8_t	byte;		//8-bit value
typedef uint16_t	word;		//16-bit value
typedef uint32_t	dword; 	//32-bit value
 
#endif