#define MMAP_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/mman.c
//      Purpose:         Handles memory allocation for pointers
// ======================================================================== */

#include <mman.h>

#include <stdlib.h>
#include <errors.h>
//#include <string.h>
#include <util.h>

// ======================================================================== */
//       Static prototypes
// ======================================================================== */

static mtable_t* const mtable = (mtable_t*)HEAP_START;

static size_t  mtable_index(addr_t);
static bool    is_addr_free(addr_t);
static bool    block_fits(addr_t,size_t);
static void    mtable_delete(size_t);
static size_t  block_end(size_t);

// ======================================================================== */
//       Public API (Note: prototypes are in stdlib.h)
// ======================================================================== */

size_t ptrsize(void* ptr)
{
   /* Search the table for this pointer */
   foreach(i, mtable->blocks) {
      if((addr_t)ptr == mtable->entry[i].start) {
         return mtable->entry[i].size;
      }
   }
   return 0;
}

void free(void* ptr)
{
   size_t index = mtable_index((addr_t)ptr);
   if(index > mtable->blocks-1) {
      return;
   }
   else mtable_delete(index);
}

/* NOTE: #define calloc(num, size) --> _calloc(__func__, num, size) */ 

void* _calloc(const char* func, size_t items, size_t size)
{
   byte* pointer = (byte*) _malloc(func, items * size);
   foreach(i, items*size) {
      pointer[i] = 0;
   }
   return (void*) pointer;
}

/* NOTE: #define malloc(size) -->  _malloc(__func__, size) */

void* _malloc(const char* func, const size_t size)
{
   if(size == 0) { 
      return NULL; 
   }
   /* Loop through memory until a free block is found */
   for(addr_t address = ALLOC_START; address<ALLOC_END; address+=size) 
   {
      /* If all addresses are free between [addr] and [addr+size] */
      if(block_fits(address, size)) {
         /* Reserve new memory block and push it to mtable */
         mtable->entry[mtable->blocks].start = address;
         mtable->entry[mtable->blocks].size  = size;
         strcpy(mtable->entry[mtable->blocks].caller, func);
         mtable->blocks++;
         return (void*)address;
      }
   }
   throw("Failed to allocate memory",2);
   return NULL;
}

/* NOTE: #define realloc(ptr, size) --> _realloc(__func__, ptr, size) */
void* _realloc(const char* func, void* ptr, size_t size)
{
   byte* newptr = (byte*) _malloc(func, size);
   byte* oldptr = (byte*) ptr;
   foreach(i, sizeof(oldptr)) {
      newptr[i] = oldptr[i];
   }
   free(oldptr);
   return (void*)newptr;
}

// ======================================================================== //
//       Memory Usage Statistics                                            //
// ======================================================================== //

/**
 *    Avian_Documentation:
 *    Returns the total number of blocks allocated
 */
size_t mem_blocks(void)
{
   return mtable->blocks;
}   

/**
 *    Avian_Documentation:
 *    Returns the total number of bytes used in memory
 */
uint32_t mem_used(void)
{
   uint32_t used=0;
   foreach(i, mtable->blocks) {
      used += mtable->entry[i].size;
   }
   return used;
}

/**
 *    Avian_Documentation:
 *    Returns the total number of unused bytes in memory
 */
uint32_t mem_free(void)
{
   return ALLOC_SIZE-mem_used();
}

/**
 *    Avian_Documentation:
 *    Sets the given address pointer and size pointer to the
 *    respective starting address and size of the memory block
 *    associated with the table index given. If the index is 
 *    not valid, the function returns a nonzero value.
 */
int mem_block_info(size_t index, addr_t* addr, size_t* size, char* caller)
{
   if(index < mtable->blocks) {
      *addr = mtable->entry[index].start;
      *size = mtable->entry[index].size;
      strcpy(caller, mtable->entry[index].caller);
      return 0;
   }
   else return 1;
}

// ======================================================================== */
//       Static helper functions
// ======================================================================== */


static inline void mtable_delete(const size_t index)
{
   mtable->entry[index] = mtable->entry[mtable->blocks-1];
   mtable->blocks = mtable->blocks -1;
}

static inline size_t mtable_index(const addr_t ptr)
{
   /* Find associated entry in mtable */
   foreach(i, mtable->blocks)
   {
      if(mtable->entry[i].start == ptr) {
         return i;
      }   
   }
   
   return SIZE_MAX;
}

static inline bool is_addr_free(const addr_t ptr)
{
   foreach(i, mtable->blocks)
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