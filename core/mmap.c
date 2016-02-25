#define MMAP_C_SOURCE
/*
 *		Avian Project - Bryan Webb
 *		File:		/core/mtable->c
 *		Purpose:	Handles memory allocation for pointers
 */
#include <mmap.h>
#include <stdlib.h>
#include <envar.h>
#include <types.h>
#include <util.h>
#include <exceptions.h>

/* Declare memory map area */
static mtable_t* const mtable = (mtable_t*)HEAP_START;


void free(void* ptr)
{
	size_t index = mtable_index((addr_t)ptr);
	if(index > mtable->blocks-1) {
		return;
	}
	else mtable_delete(index);
}



void* malloc(const size_t size)
{
	/* Loop through memory until a free block is found */
	for(addr_t address = ALLOC_START; address<ALLOC_END; address+=size) 
	{
		/* If all addresses are free between [addr] and [addr+size] */
		if(block_fits(address, size)) {
			/* Reserve new memory block and push it to mtable */
			mtable->entry[mtable->blocks].start = address;
			mtable->entry[mtable->blocks].size  = size;
			mtable->blocks++;
			return (void*)address;
		}
	}
	return 0;
}

void* memcpy(void *str1, const void *str2, size_t n)
{
	byte *source = (byte*)str2;
	byte *dest = (byte*)str1;
	
	for(int i=0; i<n; i++) {
		dest[i] = source[i];
	}
	return dest;
}

size_t mem_blocks(void)
{
	return mtable->blocks;
}	

uint32_t mem_used(void)
{
	uint32_t used=0;
	 for(size_t i=0; i<mtable->blocks; i++) {
		used += mtable->entry[i].size;
	}
	
	return used;
}

uint32_t mem_free(void)
{
	return ALLOC_SIZE-mem_used();
}

/******** Static helper functions *************/

static inline void mtable_delete(const size_t index)
{
	mtable->entry[index] = mtable->entry[mtable->blocks-1];
	mtable->blocks = mtable->blocks -1;
}

static void mtable_purge(void)
{
	/* Garbage collector */
}

static inline size_t mtable_index(const addr_t ptr)
{
	/* Find associated entry in mtable */
	for(size_t i=0; i<mtable->blocks; i++)
	{
		if(mtable->entry[i].start == ptr) {
			return i;
		}	
	}
	
	return SIZE_MAX;
}

static inline bool is_addr_free(const addr_t ptr)
{
	for(size_t i=0; i<mtable->blocks; i++)
	{
		if(ptr >= mtable->entry[i].start && ptr <= block_end(i)) { 
			return false; 
		}
		else continue;
	}
	
	return true;
}

static inline size_t block_end(const size_t index)
{
	return (size_t)(mtable->entry[index].start + mtable->entry[index].size);
}

static inline bool block_fits(const addr_t ptr, const size_t size)
{
	size_t block_size = 0;
	while(is_addr_free(ptr+block_size) && (ptr < ALLOC_START+ALLOC_SIZE))
	{
		++block_size;
		if(block_size >= size) return true; //Failsafe to prevent infinite loops
	}
	
	return false;
}