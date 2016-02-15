#ifndef mtable_H_INCLUDED
#define mtable_H_INCLUDED
#include <types.h>
/*
 *		Avian Project - Bryan Webb
 *		File:		/include/mmap.h
 *		Purpose:	Header file for mtable->c
 */
 
/* Define allocatable memory ranges */
enum __MMAP_DEFS
{
	ALLOC_ADDR 	= 0x07E00,	//Memory block starting addr
	ALLOC_SIZE 	= 0x781FF, 	//About 480 KiB of usable RAM
 	ALLOC_MAX	= 0x08000,	//32 KiB maximum block size
	HEAP_ADDR	= 0x00500,	//Starting address of memory map
	HEAP_SIZE	= 0x076FF,	//A bit less than 30 KiB
};

 
/* Memory table entry information */
typedef struct __TABLE_ENTRY 
{
	addr_t 	start;	//Starting address of memory block
	size_t 	size;		//Size of memory block in bytes
	
} mentry_t;

typedef struct __TABLE_MAP
{
	size_t 	blocks;
	mentry_t	entry[HEAP_SIZE/sizeof(mentry_t)];

} mtable_t;
 
/* Public API */
void*		malloc(size_t);	//Allocate memory block, and return pointer to block
void 		free(void*);		//De-allocate memory block asociated with given pointer

/* Internal static functions */
static size_t	mtable_index(addr_t);
static bool		is_addr_free(addr_t);
static size_t 	max_block_size(addr_t);
static void		mtable_delete(size_t);

#endif