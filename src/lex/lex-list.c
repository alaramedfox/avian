#define LEX_LIST_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/lex/lex-list.c                                    
//    Purpose:       Lex list functions                    
// ======================================================================== */

#include <lex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

void init_lex_list(void);
void lex_list(void);

static void lex_list_help(void);

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

__attribute__ ((constructor))
void init_lex_list(void)
{
   lex_add_command("list", lex_list);
}

void lex_list(int argc, char* argv[])
{
   char** args = split(' ', 0, _argv);
   foreach(i, argc) {
   
   }
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //