#define LEX_MISC_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/lex/lex-term.c                                    
//    Purpose:       Lex terminal functions                    
// ======================================================================== */

#include <lex.h>
#include <stdio.h>
#include <string.h>
#include <util.h>
#include <vga.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

static void lex_term_clear(void);
static void lex_term_color(char*, char*);
static void lex_term_help(void);
static void lex_term_tabs(char*);
static void lex_term_history(char*);

extern volatile lex_history_t history;

volatile byte lex_file_color = GREEN;
volatile byte lex_dir_color = CYAN;
volatile byte lex_sys_color = BROWN;
volatile byte lex_text_color = 0x07;

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

void lex_term(int argc, char* argv[])
{
   int a=2;
   char *arg1, *arg2;
   if(argc >= 2 && argv[1][0] == ':') {
      foreach(i, strlen(argv[1])) {
         switch(argv[1][i])
         {
            case ':': break;
            case '?': lex_term_help(); break;
            case 'C':
            case 'c': lex_term_clear(); break;
            case '#': 
               arg1 = ARGV(a);
               arg2 = ARGV(a);
               lex_term_color(arg1, arg2); 
               break;
            case 't': lex_term_tabs(ARGV(a)); break;
            case 'h': lex_term_history(ARGV(a)); break;
            
            default: break;
         }
      }
   }
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void lex_term_clear(void)
{
   vga_clear();
}

static void lex_term_tabs(char* size)
{
   if(size == NULL) {
      printf("Current tab size: %i\n",vga_tabsize(0)); 
      return;
   }
   vga_tabsize(atoi(size));
}

static void lex_term_history(char* lines)
{
   VALIDATE_ARG(lines, return);
   int num = atoi(lines);
   foreach(i, num) {
      int h = history.size-i-1;
      if(h < 0) break;
      printf("%i: %s\n",h,history.record[h]);
   }
}

static void lex_term_color_help(void)
{
   printf("Available terminal color categories:\n");
   printf("  file      Names of files, when listed\n");
   printf("  dir       Names of directories, when listed\n");
   printf("  sys       Special system-related characters\n");
   printf("  text      General, standard, basic text\n");
}

static void lex_term_color(char* category, char* hue)
{
   VALIDATE_ARG(category, return);
   VALIDATE_ARG(hue, return);
   
   byte color = atoi(hue);
        if(strcmp(category, "file") == 0) lex_file_color = color;
   else if(strcmp(category, "dir") == 0)  lex_dir_color = color;
   else if(strcmp(category, "sys") == 0)  lex_sys_color = color;
   else if(strcmp(category, "text") == 0) lex_text_color = color;
   else lex_term_color_help();
}
static void lex_term_help(void)
{
   printf("Usage: t :[options...] [values...]\n");
   printf(" ?       Print this help documentation\n");
   printf(" c       Clear the screen (Case insensitive)\n");
   printf(" # [cat] [val] Set the color of category CAT to the specified value\n");
   printf(" t [val]       Set the default tab width to the specified value\n");
   printf(" h [N]         Display the last N commands\n");
}


