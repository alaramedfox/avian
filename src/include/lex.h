#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/include/lex.h                                    
//    Purpose:       Header for the LEXTEM Command Processor                   
// ======================================================================== */

#include <stdlib.h>
#include <stdarg.h>
#include <filesystem.h>
#include <anicafs.h>

// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

#define ARGV(count) count<argc?argv[count++]:NULL

#define MAX_LEX_MOUNTS 8

#define VALIDATE_ARG(arg, expr) \
   if(arg == NULL) { printf("Missing or invalid parameter\n"); expr; }
   
#define lex_function(name) \
   int name(int argc, char* argv[])
   
#define EXPORT_LEX(cmd, arg, doc, function)  \
   char command_##function[] = cmd;          \
   char helpdoc_##function[] = doc;          \
   char argsdoc_##function[] = arg

#define IMPORT_LEX(f)                        \
   extern void f(int argc, char* argv[]);    \
   extern char command_##f[];                \
   extern char helpdoc_##f[];                \
   extern char argsdoc_##f[];                \
   lex_add_command(command_##f, argsdoc_##f, helpdoc_##f, f)
   
enum __LEX_RETURN
{
   LEX_OK = 0,
   LEX_ERROR = 1,
};

typedef struct __LEX_FS_MOUNTPOINT
{
   device_t device;
   char* point;
   volume_t* vol;
   
} packed lex_mpoint_t;

typedef struct __LEX_HISTORY
{
   char** record;
   size_t index;
   size_t size;
   size_t max;

} packed lex_history_t;

extern lex_mpoint_t mountpoints[];
extern size_t mounted_volumes;

extern volatile lex_history_t history;
extern volatile byte lex_file_color;
extern volatile byte lex_dir_color;
extern volatile byte lex_sys_color;
extern volatile byte lex_text_color;
extern char* current_directory;

volume_t*  lex_read_mountpath(const char[], char*, char*);
void  lex_add_command(char cmd[], char arg[], char doc[], void (*f)(int, char**));
void  lex_init(void);
int   shell(void);
char* lex_full_path(const char[]);

#endif