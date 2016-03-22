#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/include/random.h                                    
//    Purpose:       Header for the pseudorandom number generator                    
// ======================================================================== */

// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

int rand(void);
void srand(unsigned int);

#endif