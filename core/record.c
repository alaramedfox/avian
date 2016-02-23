#define RECORD_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/record.c
 *		Purpose:	Mantains the Kernel Table Record
 *
 *		The Kernel Table Record is a database of memory
 *		spaces that are reserved for various purposes.
 *		The Record maps allocatable memory, buffer spaces,
 *		and any other region of memory that the Kernel needs
 *		to be used for a particular purpose. 
 */
 
#include <record.h>
 
typedef struct __RECORD_ENTRY
{
	permit_t access;
	table_t *table;

} record_entry_t;

/* The Kernel Table Record starts at byte 0 of RAM */
record_entry_t * const kernel_table_record = (record_entry_t*)0x0;