#ifndef MMAN_H_INCLUDED
#define MMAN_H_INCLUDED
// ======================================================================= */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/mman.h
//      Purpose:         Header file for the memory management unit
// ======================================================================= */

#include <stdlib.h>
#define USE_UPPER_MEMORY 0

/* Define allocatable memory ranges */
enum __MMAP_DEFS
{
#if USE_UPPER_MEMORY
   ALLOC_START = 0x280000,    // 2.5 MB offset
   ALLOC_SIZE  = 0x800000,    // 8 MB of RAM
   ALLOC_END   = 0xA80000,    // Ending address of memory
#else
   ALLOC_START = 0x17E00,
   ALLOC_SIZE  = 0x681FF,
   ALLOC_END   = 0x7FFFF,
#endif
   HEAP_START  = 0x00500,   //Starting address of memory map
   HEAP_SIZE   = 0x076FF,   //A bit less than 30 KiB
   HEAP_END    = 0x07BFF,   //Ending address of heap
   
   BLOCK_MAX   = 0x01000,   //4 KiB maximum block size
   BLOCK_COUNT = 0x00EDF,  //Maximum number of blocks
};

typedef struct __TABLE_ENTRY 
{
   addr_t    start;   //Starting address of memory block
   size_t    size;      //Size of memory block in bytes
   
} FLAT mentry_t;

typedef struct __TABLE_MAP
{
   size_t    blocks;
   mentry_t   entry[BLOCK_COUNT];

} FLAT mtable_t;
 
/* Public API */

/**   
 *    NOTE: The prototypes for the memory management functions
 *      are declared in stdlib.h
 *
 *      This file is just for the memory info functions and
 *    static methods.
 */

__attribute__((hot)) size_t   mem_blocks(void); 
__attribute__((hot)) uint32_t   mem_used(void);
__attribute__((hot)) uint32_t   mem_free(void);



#endif