#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/table.h
//      Purpose:         Manage tables of data
// ======================================================================== */
 
#include <stdlib.h>

/**
 *     Definition of the different types 
 *      of data, for searching purposes 
 */
 
enum __ENTRY_TYPES
{
   RAM_TABLE,    // Table type for addressable memory
   
};

/**
 *    Each table entry is 8 bytes, and contains
 *      the starting address of the structure and
 *      the allocated size. These structures are
 *      NOT controlled by `malloc'!
 */
 
typedef struct __ENTRY
{
   addr_t    start;   // Starting address of entry
   size_t   size;      // Size of element
   
} packed entry_t;

/* Table list */
typedef struct __TABLE
{
   byte info;
   size_t size;
   size_t max;
   entry_t *entry;

} packed table_t;



#endif