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
#include <errors.h>

typedef struct __LEX_INDEX
{
   char* cmd;
   void (*function)(int argc, char* argv[]);

} lex_index_t;

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

/* Forward-declare LEX commands */
extern void lex_list(int, char**);

/* Static functions */
static void lex_execute(int, char**);
static void lex_add_command(char*, void (*function)(int argc, char* argv[]));

/* Private variables */
static const char    prompt[] = "Lex: ";
static lex_index_t   command_index[256];
static size_t        lex_index_size = 0;

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

void lex_init(void)
{
   lex_add_command("l", lex_list);
   
   throw("Initialized Lex shell",0);
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
      printf("\nA");
      chomp(input);
      //printf("\n");
      
      /* Splice the command and args from input */
      printf("B");
      char** argv = (char**) malloc(20);
      printf("C");
      int argc = split(' ', 0, input, argv);
      
      /* Send the list to the command processor */
      printf("D\n");
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

static void lex_add_command(char cmd[], void (*function)(int argc, char* argv[]))
{
   command_index[lex_index_size].cmd = cmd;
   command_index[lex_index_size].function = function;
   lex_index_size++;
}


