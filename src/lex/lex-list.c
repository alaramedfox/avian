#define LEX_LIST_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/lex/lex-list.c                                    
//    Purpose:       Lex list functions                    
// ======================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mman.h>
#include <util.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

void lex_list(int, char**);

static void lex_list_help(void);
static void lex_list_dir(void);
static void lex_list_memory(void);
static void lex_list_unknown(char);
static void lex_list_vars(void);

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

void lex_list(int argc, char* argv[])
{
   if(argc >= 2 && argv[1][0] == ':') {
      foreach(i, strlen(argv[1])) {
         switch(argv[1][i])
         {
            case ':': break;
            case '?': lex_list_help(); break;
            case 'm': lex_list_memory(); break;
            case 'd': lex_list_dir(); break;
            case 'v': lex_list_vars(); break;
            
            default: lex_list_unknown(argv[1][0]); break;
         }
      }
   }
   else {
      lex_list_help();
   }
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void lex_list_help(void)
{
   printf("Usage: l :[category ...]  - Lists items in the given category\n");
   printf(" ?     Print this help information\n");
   printf(" m     Memory usage statistics\n");
   printf(" d     Contents of current directory\n");
   printf(" v     Print the size of internal variables\n");
}

static void lex_list_memory(void)
{
   printf("  Total     %h\t(%i bytes)\n",ALLOC_SIZE, ALLOC_SIZE);
   printf("  Used      %h\t(%i bytes)\n",mem_used(), mem_used());
   printf("  Free      %h\t(%i bytes)\n",mem_free(), mem_free());
   printf("  Blocks    %i\n\n",mem_blocks());
}

static void lex_list_vars(void)
{
   types_test();
}

static void lex_list_dir(void)
{

}

static void lex_list_unknown(char category)
{
   printf("Unknown category `%c'\n",category);
}


