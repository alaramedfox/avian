#define FILE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/lex/lex.c                                   
//    Purpose:       Main point for the LEXTEM Shell
//
//    LEXTEM is the Avian Shell -- it stands for the basic commands:
//    List, EXecute, Transfer, Edit, M-something                  
// ======================================================================== */

#include <lex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

static const char prompt[] = "Lex: ";

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

/**
 *    Main Shell Loop
 *    Lex will provide a prompt, then gather input, filter
 *    it for commands and arguments, then passes it to the
 *    correct program. 
 */
int shell(void)
{
   char* input;
   while(true)
   {
      input = (char*) calloc(80,1);
      printf("%s",prompt);
      scan(input);
      chomp(input);
      printf("`%s'\n",input);
      free(input);
   }
   return 0;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //