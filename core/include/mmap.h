#ifndef mtable_H_INCLUDED
#define mtable_H_INCLUDED
#include <types.h>
/*
 *		Avian Project - Bryan Webb
 *		File:		/include/mmap.h
 *		Purpose:	Header file for the MMU
 */

/* Define allocatable memory ranges */
enum __MMAP_DEFS
{
	ALLOC_START = 0x07E00,	//Memory block starting addr
	ALLOC_SIZE 	= 0x781FF, 	//About 480 KiB of usable RAM
	ALLOC_END   = 0x7FFFF,	//Ending address of free memory
	
 	HEAP_START	= 0x00500,	//Starting address of memory map
	HEAP_SIZE	= 0x076FF,	//A bit less than 30 KiB
	HEAP_END		= 0x07BFF,	//Ending address of heap
	
	BLOCK_MAX	= 0x01000,	//4 KiB maximum block size
	BLOCK_COUNT = 0x00EDF,  //Maximum number of blocks
};

typedef struct __TABLE_ENTRY 
{
	addr_t 	start;	//Starting address of memory block
	size_t 	size;		//Size of memory block in bytes
	
} mentry_t;

typedef struct __TABLE_MAP
{
	size_t 	blocks;
	mentry_t	entry[BLOCK_COUNT];

} mtable_t;
 
/* Public API */

/*		
 *		NOTE: The prototypes for the memory management functions
 *				are declared in stdlib.h
 *
 *				This file is just for the memory info functions and
 *				static methods.
 */
#define  new(T) (T*)malloc(sizeof(T))

__attribute__((hot)) size_t	mem_blocks(void); 
__attribute__((hot)) uint32_t	mem_used(void);
__attribute__((hot)) uint32_t	mem_free(void);

/* Internal static functions */
static size_t	mtable_index(addr_t);
static bool		is_addr_free(addr_t);
static bool 	block_fits(addr_t,size_t);
static void		mtable_delete(size_t);
static void		mtable_purge(void);
static size_t  block_end(size_t);
static void    mtable_error(const char*);

#endif