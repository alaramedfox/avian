#define MMAP_C_SOURCE
#include <mmap.h>
/*
 *		Popcorn Kernel - Bryan Webb
 *		File:		/core/mmap.c
 *		Purpose:	Handles memory allocation for pointers
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

/* Create object for memory map */
typedef struct __ENTRY 
{
	address_t start;
	size_t size;
} mmap_t;

/* Global memory map variables */
mmap_t 	*const mmap = (mmap_t*)HEAP_ADDR;	//Declare memory map area at the heap address
byte 		HEAP_BLOCK[HEAP_SIZE];					//Dummy filler block of bytes
size_t 	blocks_allocated=0;						//Keep track of number of allocated blocks

void free(void* ptr)
{
	mmap_delete( mmap_index((address_t)ptr) );
}

void mmap_delete(const size_t index)
{
	for(size_t i=index; i<blocks_allocated-1; i++) {
		mmap[i] = mmap[i+1];
	}
}

size_t mmap_index(const address_t ptr)
{
	/* Find associated entry in mmap */
	for(size_t i=0; i<blocks_allocated; i++) {
		if(mmap[i].start == ptr) {
			return i;
		}	
	}
	return 0;
}

void* malloc(const size_t size)
{
	/* Loop through memory until a free block is found */
	for(address_t address = ALLOC_ADDR; address<ALLOC_SIZE; address++) 
	{
		/* If all addresses are free between [addr] and [addr+size] */
		if(max_block_size(address) > size) {
			/* Reserve new memory block and push it to mmap */
			mmap[blocks_allocated].start = address;
			mmap[blocks_allocated].size  = size;
			++blocks_allocated;
			return (void*)address;
		}
		else {
			/* Skip forward by size, because we already know this cannot fit here */
			address += size;
			//Note: This might be terribly wrong!
		}
	}
}

bool is_addr_free(const address_t ptr)
{
	for(size_t i=0; i<blocks_allocated; i++)
	{
		if(ptr >= mmap[i].start && ptr <= (mmap[i].start+mmap[i].size)) { 
			return false; 
		}
		else continue;
	}
	return true;
}

size_t max_block_size(const address_t ptr)
{
	size_t size = 0;
	while(is_addr_free(ptr+size)) 
	{
		++size;
		if(size > ALLOC_MAX) break; //Failsafe to prevent infinite loops
	}
	return size;
}