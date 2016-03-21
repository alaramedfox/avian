#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/include/lex.h                                    
//    Purpose:       Header for the LEXTEM Command Processor                   
// ======================================================================== */

#include <stdlib.h>

// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

void lex_add_command(char*, void (*function)(char[])); 
int shell(void);

#endif