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

static void lex_execute(char*,char*);

static void lex_test(char args[])
{
   printf("The arguments are: %s\n",args);
}

static const char prompt[] = "Lex: ";

typedef struct __LEX_INDEX
{
   char* cmd;
   void (*function)(char args[]);

} lex_index_t;

static lex_index_t command_index[256];
static size_t lex_index_size = 0;

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
   lex_add_command("test",lex_test);
   
   char* input;
   while(true)
   {
      printf("%s",prompt);
      
      /* Gather and chomp the input */
      input = (char*) calloc(80,1);
      scan(input);
      chomp(input);
      
      /* Splice the command and args from input */
      char* command = split(' ', 0, input)[0];
      char* args = (input+strlen(command)+1);
      
      /* Send the list to the command processor */
      lex_execute(command, args);
      
      //printf("`%s'\n",input);
      free(input);
   }
   return 0;
}

void lex_add_command(char cmd[], void (*function)(char args[]))
{
   command_index[lex_index_size].cmd = cmd;
   command_index[lex_index_size].function = function;
   lex_index_size++;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void lex_execute(char* cmd, char* args)
{
   foreach(i, lex_index_size) {
      if(strcmp(cmd, command_index[i].cmd) == 0) {
         command_index[i].function(args);
         return;
      }
   }
   printf("Unknown command `%s'\n",cmd);
}



