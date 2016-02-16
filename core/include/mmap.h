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
 	ALLOC_MAX	= 0x01000,	//4 KiB maximum block size
	HEAP_ADDR	= 0x00500,	//Starting address of memory map
	HEAP_SIZE	= 0x076FF,	//A bit less than 30 KiB
	MAX_BLOCKS  = 0x00EDF,  //Maximum number of blocks
};

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

size_t		mem_blocks(void); //Return number of allocated blocks
uint32_t		mem_used(void);	//Return amount of used memory
uint32_t		mem_free(void);	//Return amount of free memory

/* Internal static functions */
static size_t	mtable_index(addr_t);
static bool		is_addr_free(addr_t);
static bool 	block_fits(addr_t,size_t);
static void		mtable_delete(size_t);
static void		mtable_purge(void);
static size_t  block_end(size_t);
static void    mtable_error(const char*);

#endif