#define FILE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/trace.c                                   
//    Purpose:       Provide utilities to trace functions                    
// ======================================================================== */

#include <trace.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

static char this_function[28];

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

void record_function_exit(const char function[])
{
   
}

void record_function_call(const char function[])
{
   int i=0;
   for(i=0; function[i] != '\0'; i++) {
      this_function[i] = function[i];
   }
   this_function[i] = '\0';
}

char* get_last_function(void)
{
   return this_function;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //