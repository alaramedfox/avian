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
#include <vga.h>

typedef struct __LEX_INDEX
{
   char* cmd;
   char* arg;
   char* doc;
   void (*function)(int argc, char* argv[]);

} lex_index_t;

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

/* Static functions */
static void lex_help(int, char**);
static void lex_execute(int, char**);
static void lex_add_history(char*);
static void lex_history_up(char*);
static void lex_history_down(char*);


/* Private variables */
static char**        prompt;
static lex_index_t*  command_index;
static size_t        lex_index_size = 0;

char* current_directory;

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

volatile lex_history_t history;
lex_mpoint_t mountpoints[MAX_LEX_MOUNTS];
size_t mounted_volumes=0;

void lex_init(void)
{
   history.index = 0;
   history.max = 255;
   history.record = (char**) malloc(history.max);
   
   current_directory = (char*) calloc(128,1);
   prompt = (char**) malloc(4);
   
   command_index = (lex_index_t*) malloc(256 * sizeof(lex_index_t));
   
   IMPORT_LEX(lex_help);
   IMPORT_LEX(lex_clear);
   IMPORT_LEX(lex_ls);
   IMPORT_LEX(lex_format);
   IMPORT_LEX(lex_make);
   IMPORT_LEX(lex_devlist);
   IMPORT_LEX(lex_mount);
   IMPORT_LEX(lex_umount);
   IMPORT_LEX(lex_fsdump);
   IMPORT_LEX(lex_varstat);
   IMPORT_LEX(lex_memstat);
   IMPORT_LEX(lex_enter);
   IMPORT_LEX(lex_fscache);
   
   throw("Initialized Lex shell",0);
}

/**
 *    Avian_Documentation:
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
 *    Avian_Documentation:
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
      printf("%A(lex)%s%A ",BOLD, prompt[1]==NULL?"root:":prompt[1], TERM);
      
      
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

/**
 *    Avian_Documentation:
 *    Converts the provided relative path into the full path, based on
 *    the current working directory string.
 */
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
   
   return new_path;
}


/**
 *    Avian_Documentation:
 *    Registers the provided function pointer with the provided command.
 *    These additions should be loaded using the __constructor__ GCC
 *    attribute.
 */
void lex_add_command(char cmd[], char arg[], char doc[], 
                     void (*function)(int argc, char* argv[]))
{
   command_index[lex_index_size].cmd = cmd;
   command_index[lex_index_size].arg = arg;
   command_index[lex_index_size].doc = doc;
   command_index[lex_index_size].function = function;
   lex_index_size++;
}

/**
 *    Avian_Documentation:
 *    Reads a given path, and splits it apart to work out the mountpoint,
 *    for which it reads the volume_t structure, populates the mountpoint
 *    string, and populates the real_path string. Upon success, returns true.
 *    False otherwise.
 */
volume_t* lex_read_mountpath(const char path[], char* point, char* real_path)
{
   volume_t* ptr;
   /* Split the path in the form "Point:Path" */
   char** split_path = (char**) malloc(8);
   int halves = split(':',' ',path,split_path);
   if(halves != 2) {
      printf("Paths must be of the form `Mountpoint:path'\n");
      ptr = NULL;
      goto exit;
   }
   
   /* Extract the two components of the path */
   strcpy(point, split_path[0]);
   strcpy(real_path, split_path[1]);
   
   /* Aquire the device to be accessed */
   device_t device = NO_DEV;
   foreach(i, mounted_volumes) {
      if(strcmp(point, mountpoints[i].point) == 0) {
         device = mountpoints[i].device;
         ptr = mountpoints[i].vol;
         break;
      }
   }
   if(device == NO_DEV) {
      printf("%s is not mounted\n",point);
      ptr = NULL;
      goto exit;
   }
   
   exit:
   foreach(i, halves) free(split_path[i]);
   free(split_path);
   return ptr;
}

EXPORT_LEX("help", "[command]", "Display specific help information", lex_help);
void lex_help(int argc, char* argv[])
{
   printf("LEX Shell version 0.1 -- Command overview\n\n");
   printf("-CMD------ARGS---------------INFO----------------------\n");
   foreach(i, lex_index_size) {
      vga_movexy(vga_getrow(), 1);  printf("%s", command_index[i].cmd);
      vga_movexy(vga_getrow(), 10); printf("%s", command_index[i].arg);
      vga_movexy(vga_getrow(), 30); printf("%s", command_index[i].doc);
      printf("\n");
   }
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

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

static void lex_history_up(char* buffer)
{
   if(history.index > 0) {
      char* previous = history.record[--history.index];
      strcpy(buffer, previous);
   }
   else return; 
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




