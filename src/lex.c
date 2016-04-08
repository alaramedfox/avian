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
#include <keyboard.h>

typedef struct __LEX_INDEX
{
   char* cmd;
   char* alias;
   void (*function)(int argc, char* argv[]);

} lex_index_t;

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

/* Forward-declare LEX commands */
extern void lex_list(int, char**);
extern void lex_term(int, char**);
extern void lex_manf(int, char**);

/* Static functions */
static void lex_help(int, char**);
static void lex_execute(int, char**);
static void lex_add_command(char*, char*, void (*f)(int, char**));
static void lex_add_history(char*);
static void lex_history_up(char*);
static void lex_history_down(char*);

/* Private variables */
static const char    prompt[] = "Lex: ";
static lex_index_t*  command_index;
static size_t        lex_index_size = 0;

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

volatile lex_history_t history;

void lex_init(void)
{
   history.index = 0;
   history.max = 25;
   history.record = (char**) malloc(history.max);
   
   command_index = (lex_index_t*) malloc(256 * sizeof(lex_index_t));
   
   lex_add_command("l", "list", lex_list);
   lex_add_command("t", "term", lex_term);
   lex_add_command("?", "help", lex_help);
   lex_add_command("fs", "manfs", lex_manf);
   
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
   register_scan_event(UARROW, lex_history_up, false);
   register_scan_event(DARROW, lex_history_down, false);
   char* input;
   while(true)
   {
      printf("%s",prompt);
      
      /* Gather and chomp the input */
      input = (char*) calloc(80,1);
      scan(input);
      chomp(input);
      
      lex_add_history(input);
      
      printf("\n");
      
      /* Splice the command and args from input */
      char** argv = (char**) malloc(20);
      int argc = split(' ', 0, input, argv);
      
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



// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void lex_history_up(char* buffer)
{
   if(history.index > 0) {
      char* previous = history.record[--history.index];
      strcpy(buffer, previous);
   }
   else return; 
}

static void lex_history_down(char* buffer)
{
   if(history.index < history.size-1) {
      char* next = history.record[++history.index];
      strcpy(buffer, next);
   }
   else {
      history.index = history.size;
      memset(buffer, 0, strlen(buffer));
   }
}

static void lex_add_history(char* line)
{
   if(history.size < history.max) {
      /* Do not add if this command is identical to the previous one */
      if(strcmp(history.record[history.size-1], line) == 0) return;
      history.record[history.size] = (char*) malloc(strlen(line));
      strcpy(history.record[history.size], line);
      history.size++;
      history.index = history.size;
   }
   else {
      foreach(i, history.size-1) {
         history.record[i] = history.record[i+1];
      }
      free(history.record[history.size]);
      history.record[history.size] = (char*) malloc(strlen(line));
      strcpy(history.record[history.size], line);
   }
}

static void lex_help(int argc, char* argv[])
{
   printf("LEX Shell version 0.1 -- Command overview\n\n");
   printf("-CMD----ALIAS----ARGS------------------INFO----------------------\n");
   printf(" l      list     :[cat...] [val...]    List items in a category\n");
   printf(" e      edit     :[mode]   [object]    Edit files, variables, etc\n");
   printf(" x      exec     :[mode]   [program]   Execute a program\n");
   printf(" t      term     :[var...] [val...]    Manipulate the terminal\n");
   printf(" fs     manfs    :[opt...] [val...]    Manage the filesystem\n");
   printf("\n");
   printf("For more details on a command, type `[command] :?'\n");
}

static void lex_execute(int argc, char* argv[])
{
   if(argc == 0) return;
   foreach(i, lex_index_size) {
      if(strcmp(argv[0], command_index[i].cmd) == 0 ||
         strcmp(argv[0], command_index[i].alias) == 0) {
         command_index[i].function(argc, argv);
         return;
      }
   }
   printf("Unknown command `%s'\n",argv[0]);
}

static void lex_add_command(char* cmd, char* alias, void (*function)(int argc, char* argv[]))
{
   command_index[lex_index_size].cmd = cmd;
   command_index[lex_index_size].alias = alias;
   command_index[lex_index_size].function = function;
   lex_index_size++;
}


