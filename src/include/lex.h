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

#define VALIDATE_ARG(arg, expr) \
   if(arg == NULL) { printf("Missing or invalid parameter\n"); expr; }

typedef struct __LEX_HISTORY
{
   char** record;
   size_t index;
   size_t size;
   size_t max;

} packed lex_history_t;

extern volatile lex_history_t history;
extern volatile byte lex_file_color;
extern volatile byte lex_dir_color;
extern volatile byte lex_sys_color;
extern volatile byte lex_text_color;
extern char* current_directory;

void lex_init(void);
int shell(void);
char* lex_full_path(const char[]);

#endif