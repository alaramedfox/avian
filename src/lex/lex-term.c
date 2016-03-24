#define LEX_MISC_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/lex/lex-term.c                                    
//    Purpose:       Lex terminal functions                    
// ======================================================================== */

#include <stdio.h>
#include <string.h>
#include <util.h>
#include <vga.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

static void lex_term_clear(void);
static void lex_term_color(byte);
static void lex_term_help(void);
static void lex_term_tabs(byte);

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

void lex_term(int argc, char* argv[])
{
   int arg_loc=2;
   if(argc >= 2 && argv[1][0] == ':') {
      foreach(i, strlen(argv[1])) {
         switch(argv[1][i])
         {
            case ':': break;
            case '?': lex_term_help(); break;
            case 'C':
            case 'c': lex_term_clear(); break;
            case '#': lex_term_color(atoi(argv[arg_loc++])); break;
            case 't': lex_term_tabs(atoi(argv[arg_loc++])); break;
            
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

static void lex_term_tabs(byte size)
{
   vga_tabsize(size);
}

static void lex_term_color(byte color)
{
   int loc = vga_getloc();
   vga_moveptr(0);
   foreach(i, VGA_LEN) {
      vga_color(color);
      vga_increment();
   }
   vga_setcolor(color);
   vga_moveptr(loc);
}
static void lex_term_help(void)
{
   printf("Usage: t :[options...] [values...]\n");
   printf(" ?       Print this help documentation\n");
   printf(" c       Clear the screen (Case insensitive)\n");
   printf(" # [val] Set the terminal color to the specified value\n");
   printf(" t [val] Set the default tab width to the specified value\n");
}


