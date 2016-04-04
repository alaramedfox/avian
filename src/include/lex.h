#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/include/lex.h                                    
//    Purpose:       Header for the LEXTEM Command Processor                   
// ======================================================================== */

#include <stdlib.h>
#include <stdarg.h>

// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

#define ARGV(count) count<argc?argv[count++]:NULL

void lex_init(void);
int shell(void);

#endif