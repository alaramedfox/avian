#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
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

#if !defined __SELECT_TYPES__
	#define __FIXED_WIDTH_TYPES__
	#define __VERBOSE_TYPES__
	#define __DATA_TYPES__
#endif

#if defined __FIXED_WIDTH_TYPES__
	/* These defines re-declare the common `int', `short', etc
	 * datatypes to constant-width types. It is preferred to use
	 * `word' and `dword', but these are here for compatability and such.
	 */
	#define short		int16_t
	#define int			int32_t
	#define long		int32_t
#endif

typedef enum __STATUS
{
	RETRY, FAIL, ABORT, OK, READY,
	
} status_t;

/* Define a list of usable ports for bus IO */
typedef enum __PORTS
{
	/* Programmable Interrupt Controller */
	PIC1_CMD = 0x20, PIC1_DAT = 0x21, PIC2_CMD = 0xA0, PIC2_DAT = 0xA1,
	
	/* PS/2 Controller - Keyboard & Mice */
	KB_DATA = 0x60, KB_STATUS = 0x64,
	
	/* Floppy Drive Controller */
	FDC_SRA = 0x3f0, FDC_SRB = 0x3f1, FDC_MSR  = 0x3f4, FDC_DIR = 0x3f7,
	FDC_DSR = 0x3f4, FDC_CCR = 0x3f7, FDC_FIFO = 0x3f5, FDC_DOR = 0x3f2,

} port_t;

#if defined __VERBOSE_TYPES__
	/* These types are for use in clarity of function arguments.
	 * They indicate the purpose of said arguments.
	 */
	typedef uint16_t 	index_t;
	typedef uintptr_t addr_t;
#endif

#if defined __DATA_TYPES__
	/* These types are for easy data size recognition.
	 * They are intended for use in data transfer between
	 * components, where a specific data size is required, 
	 * such as port IO or data structures.
	 */
	typedef uint8_t	byte;		//8-bit value		1 byte
	typedef uint16_t	word;		//16-bit value		2 bytes
	typedef uint32_t	dword; 	//32-bit value		4 bytes
#endif

#endif