#ifndef RECORD_H_INCLUDED
#define RECORD_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/record.h
 *		Purpose:	Mantains the Kernel Table Record
 *
 *		The Kernel Table Record is a database of memory
 *		spaces that are reserved for various purposes.
 *		The Record maps allocatable memory, buffer spaces,
 *		and any other region of memory that the Kernel needs
 *		to be used for a particular purpose. 
 */
#include <types.h>
#include <table.h>
#include <permissions.h>

/* Table addition functions */
void record_add_table(permit_t, table_t*);

/* Information request */


#endif