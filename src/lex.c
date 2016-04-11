#define LEX_C_SOURCE
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
static void lex_add_command(char*, void (*f)(int, char**));
static void lex_add_history(char*);
static void lex_history_up(char*);
static void lex_history_down(char*);

/* Functions for internal aliases */
static void lex_alias_clear(int, char**);
static void lex_alias_enter(int, char**);
static void lex_alias_make(int, char**);
static void lex_alias_dirlist(int, char**);


/* Private variables */
static char**        prompt;
static lex_index_t*  command_index;
static size_t        lex_index_size = 0;

char* current_directory;

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

volatile lex_history_t history;

void lex_init(void)
{
   history.index = 0;
   history.max = 255;
   history.record = (char**) malloc(history.max);
   
   current_directory = (char*) calloc(128,1);
   prompt = (char**) malloc(4);
   
   command_index = (lex_index_t*) malloc(256 * sizeof(lex_index_t));
   
   lex_add_command("list", lex_list);
   lex_add_command("term", lex_term);
   lex_add_command("help", lex_help);
   lex_add_command("manfs", lex_manf);
   
   lex_add_command("clear", lex_alias_clear);
   lex_add_command("enter", lex_alias_enter);
   lex_add_command("make", lex_alias_make);
   lex_add_command("ld", lex_alias_dirlist);
   
   throw("Initialized Lex shell",0);
}

/**
 *    Anica_Documentation:
 *    Implementation of the C standard's system() function.
 *    it processes the string input and sends it to the Lex
 *    command processor. As specified by the C Standard, the
 *    function returns -1 on error, or the status of the command
 *    otherwise.
 */
int system(const char *command)
{
   char** argv = (char**) malloc(80);
   int argc = split(' ',0,command, argv);
   lex_execute(argc, argv);
   return 0;
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
      char cdir[128];
      strcpy(cdir, current_directory);
      split(':',0,cdir, prompt);
      printf("%#(lex)%s%# ",0x0F, prompt[1]==NULL?"root:":prompt[1], 0x07);
      
      
      /* Gather and chomp the input */
      input = (char*) calloc(80,1);
      scan(input);
      chomp(input);
      
      if(strlen(input) > 1) lex_add_history(input);
      
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
   deregister_scan_event(UARROW);
   deregister_scan_event(DARROW);
   return 0;
}

char* lex_full_path(const char relpath[])
{
   char* new_path = (char*) malloc(128);
   
   if(relpath == NULL || relpath[0] == 0) {
      strcpy(new_path, current_directory);
   }
   
   else if(split(':',0,relpath,NULL) < 2) {
      strcpy(new_path, current_directory);
      strcat(new_path, relpath);
   }
   else {
      strcpy(new_path, relpath);
   }
   
   //if(new_path[strlen(new_path)-1] != '/') {
   //   strcat(new_path, "/");
   //}
   return new_path;
}



// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void lex_alias_clear(int argc, char* argv[])
{
   char* command = (char*) malloc(80);
   strcpy(command, "term :c");
   strmerge(command, argv+1, argc-1, " ");
   
   system(command);
   free(command);
}

static void lex_alias_make(int argc, char* argv[])
{
   char* command = (char*) malloc(80);
   strcpy(command, "manfs :n");
   strmerge(command, argv+1, argc-1, " ");
   
   system(command);
   free(command);
}

static void lex_alias_enter(int argc, char* argv[])
{
   char* command = (char*) malloc(80);
   strcpy(command, "manfs :e");
   strmerge(command, argv+1, argc-1, " ");
   
   system(command);
   free(command);
}

static void lex_alias_dirlist(int argc, char* argv[])
{
   char* command = (char*) malloc(80);
   strcpy(command, "manfs :l");
   strmerge(command, argv+1, argc-1, " ");
   
   system(command);
   free(command);
}

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
   printf("-CMD------ARGS---------------INFO----------------------\n");
   printf(" list     :[cat..] [val..]   List items in a category\n");
   printf(" edit     :[mode]  [obj]     Edit files, variables, etc\n");
   printf(" exec     :[mode]  [app]     Execute a program\n");
   printf(" term     :[var..] [val..]   Manipulate the terminal\n");
   printf(" manfs    :[opt..] [val..]   Manage the filesystem\n");
   printf(" clear    -none-             Clear the screen\n");
   printf(" enter    [path]             Enter a subdirectory\n");
   printf(" make     :[obj..] [path..]  Create an object at the given path\n");
   printf(" ld       [path]             List contents of a directory\n");
   printf("\n");
   printf("For more details on a command, type `[command] :?'\n");
}

static void lex_execute(int argc, char* argv[])
{
   if(argc == 0 || argv == NULL || argv[0] == NULL) return;
   
   foreach(i, lex_index_size) {
      if(strcmp(argv[0], command_index[i].cmd) == 0) {
         command_index[i].function(argc, argv);
         return;
      }
   }
   printf("Unknown command `%s'\n",argv[0]);
}

static void lex_add_command(char* cmd, void (*function)(int argc, char* argv[]))
{
   command_index[lex_index_size].cmd = cmd;
   command_index[lex_index_size].function = function;
   lex_index_size++;
}


