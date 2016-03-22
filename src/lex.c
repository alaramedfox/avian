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
#include <stdio.h>
#include <string.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

static void lex_execute(int, char**);

typedef struct __LEX_INDEX
{
   char* cmd;
   void (*function)(int argc, char* argv[]);

} lex_index_t;

static const char prompt[] = "Lex: ";
static lex_index_t command_index[256];
static size_t lex_index_size = 0;

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

void lex_init(void)
{
   
}

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
      printf("%s",prompt);
      
      /* Gather and chomp the input */
      input = (char*) calloc(80,1);
      scan(input);
      chomp(input);
      
      /* Splice the command and args from input */
      char** argv = (char**) malloc(20);
      int argc = split('-', ' ', input, argv);
      
      foreach(i, argc) {
         printf("[%s] ",argv[i]);
      }
      printf("\n");
      
      /* Send the list to the command processor */
      lex_execute(argc, argv);
      
      /* Clean up */
      foreach(i, argc) {
         free(argv[i]);
      }
      free(argv);
      free(input);
   }
   return 0;
}

void lex_add_command(char cmd[], void (*function)(int argc, char* argv[]))
{
   command_index[lex_index_size].cmd = cmd;
   command_index[lex_index_size].function = function;
   lex_index_size++;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void lex_execute(int argc, char* argv[])
{
   foreach(i, lex_index_size) {
      if(strcmp(argv[0], command_index[i].cmd) == 0) {
         command_index[i].function(argc, argv);
         return;
      }
   }
   printf("Unknown command `%s'\n",argv[0]);
}



