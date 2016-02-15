#define MMAP_C_SOURCE
#include <mmap.h>
/*
 *		Avian Project - Bryan Webb
 *		File:		/core/mtable->c
 *		Purpose:	Handles memory allocation for pointers
 */
 
/* Declare memory map area */
static mtable_t* const mtable = (mtable_t*)HEAP_ADDR;

void free(void* ptr)
{
	mtable_delete( mtable_index((addr_t)ptr) );
}

void* malloc(const size_t size)
{
	/* Loop through memory until a free block is found */
	for(addr_t address = ALLOC_ADDR; address<ALLOC_SIZE; address++) 
	{
		/* If all addresses are free between [addr] and [addr+size] */
		if(max_block_size(address) > size) {
			/* Reserve new memory block and push it to mtable */
			mtable->entry[mtable->blocks].start = address;
			mtable->entry[mtable->blocks].size  = size;
			mtable->blocks++;
			return (void*)address;
		}
		else {
			/* Skip forward by size, because we already know this cannot fit here */
			address += size;
			//Note: This might be terribly wrong!
		}
	}
}

static void mtable_delete(const size_t index)
{
	for(size_t i=index; i<mtable->blocks-1; i++) {
		mtable->entry[i] = mtable->entry[i+1];
	}
	mtable->blocks--;
}

static size_t mtable_index(const addr_t ptr)
{
	/* Find associated entry in mtable */
	for(size_t i=0; i<mtable->blocks; i++)
	{
		if(mtable->entry[i].start == ptr) {
			return i;
		}	
	}
	return 0;
}

static bool is_addr_free(const addr_t ptr)
{
	for(size_t i=0; i<mtable->blocks; i++)
	{
		if(ptr >= mtable->entry[i].start && ptr <= 
			(mtable->entry[i].start + mtable->entry[i].size)) { 
			return false; 
		}
		else continue;
	}
	return true;
}

static size_t max_block_size(const addr_t ptr)
{
	size_t size = 0;
	while(is_addr_free(ptr+size)) 
	{
		++size;
		if(size > ALLOC_MAX) return ALLOC_MAX; //Failsafe to prevent infinite loops
	}
	return size;
}