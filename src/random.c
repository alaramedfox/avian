#define FILE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/random.c                                   
//    Purpose:       Pseudo-random number generator     
//                   (Taken directly from the C Standard)             
// ======================================================================== */


#include <stdlib.h>

// The following functions define a portable implementation of rand and srand.

static uint32_t next = 1;
 
int rand(void) // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (uint32_t)(next / 65536) % 32768;
}
 
void srand(uint32_t seed)
{
    next = seed;
}