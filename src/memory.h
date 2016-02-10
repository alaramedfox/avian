#ifndef MEMORY_H_INLCUDED
#define MEMORY_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		memory.h
 *			Purpose:	Handles memory allocation and prevents collision
 *						of dynamic memory
 */
 
 
/* Define address type */
//typedef unsigned long int addr_t;  //addr_t represents the integer value of void*

/* Define allocatable memory ranges */
#define ALLOC_ADDR	0x07E00	//Memory block starting addr
#define ALLOC_SIZE	0x781FF 	//About 480 KiB of usable RAM
#define ALLOC_MAX		0x08000	//32 KiB maximum block size
#define HEAP_ADDR		0x00500	//Starting address of memory map
#define HEAP_SIZE		0x076FF	//A bit less than 30 KiB

/* Create object for memory map */
struct Map {
	address_t start;
	size_t size;
};

/* Global memory map variables */
Map 		*const mmap = (Map*)HEAP_ADDR;	//Declare memory map area at the heap address
uint8_t 	HEAP_BLOCK[HEAP_SIZE];				//Dummy filler block of bytes
size_t 	blocks_allocated=0;					//Keep track of number of allocated blocks

/* Memory handling functions */
index_t	__mmap_index(address_t);		//Returns the mmap index of a given memory block
bool		__is_addr_free(address_t);		//Returns true if address is not inside allocated block
size_t 	__max_block_size(address_t);	//Returns maximum memory block size for given address
void		__mmap_delete(index_t);			//Deletes entry in mmap
void*		malloc(size_t);					//Allocate memory block, and return pointer to block
void*		malloc(size_t,address_t);		//Allocate memory block at specific address
void 		free(void*);	   				//De-allocate memory block asociated with given pointer


void free(void* ptr)
{
	__mmap_delete(__mmap_index((address_t)ptr));
}

void __mmap_delete(const index_t index)
{
	for(index_t i=index; i<blocks_allocated-1; i++)
	{
		mmap[i] = mmap[i+1];
	}
}

index_t __mmap_index(const address_t ptr)
{
	/* Find associated entry in mmap */
	for(index_t i=0; i<blocks_allocated; i++)
	{
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
		if(__max_block_size(address) > size) {
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

bool __is_addr_free(const address_t ptr)
{
	for(index_t i=0; i<blocks_allocated; i++)
	{
		if(ptr >= mmap[i].start && ptr <= (mmap[i].start+mmap[i].size)) { 
			return false; 
		}
		else continue;
	}
	return true;
}

size_t __max_block_size(const address_t ptr)
{
	size_t size = 0;
	while(__is_addr_free(ptr+size)) 
	{
		++size;
		if(size > ALLOC_MAX) break; //Failsafe to prevent infinite loops
	}
	return size;
}

#endif