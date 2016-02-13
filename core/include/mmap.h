#ifndef MMAP_H_INCLUDED
#define MMAP_H_INCLUDED
#include <types.h>
/*
 *		Popcorn Kernel - Bryan Webb
 *		File:		/include/mmap.h
 *		Purpose:	Header file for mmap.c
 */

size_t	mmap_index(address_t);		//Returns the mmap index of a given memory block
bool		is_addr_free(address_t);		//Returns true if address is not inside allocated block
size_t 	max_block_size(address_t);	//Returns maximum memory block size for given address
void		mmap_delete(size_t);			//Deletes entry in mmap
void*		malloc(size_t);					//Allocate memory block, and return pointer to block
void 		free(void*);						//De-allocate memory block asociated with given pointer

#endif