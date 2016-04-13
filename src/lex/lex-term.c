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

volatile byte lex_file_color = GREEN;
volatile byte lex_dir_color = CYAN;
volatile byte lex_sys_color = BROWN;
volatile byte lex_text_color = 0x07;

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

/**
 *    Avian_Documentation:
 *    Lex Command - term-tabs [size]
 *    Sets the tab width for the terminal
 */
EXPORT_LEX("term-tabs", lex_term_tabs);
void lex_term_tabs(int argc, char* argv[])
{
   
}


/**
 *    Avian_Documentation:
 *    Lex Command - lex-history [option]
 *    Specifies how command history is recorded
 */
EXPORT_LEX("term-history", lex_term_history);
void lex_term_history(int argc, char* argv[])
{
   
}



/**
 *    Avian_Documentation:
 *    Lex Command - clear
 *    Clears the screen
 */
EXPORT_LEX("clear", lex_clear);
void lex_clear(int argc, char* argv[])
{
   vga_clear();
}


/**
 *    Avian_Documentation:
 *    Lex Command - term-color [category] [value]
 *    Specifies the default colors used in the terminal
 */
EXPORT_LEX("term-color", lex_term_color);
void lex_term_color(int argc, char* argv[])
{
   //VALIDATE_ARG(category, return);
   //VALIDATE_ARG(hue, return);
   
   //byte color = atoi(hue);
   //     if(strcmp(category, "file") == 0) lex_file_color = color;
   //else if(strcmp(category, "dir") == 0)  lex_dir_color = color;
   //else if(strcmp(category, "sys") == 0)  lex_sys_color = color;
   //else if(strcmp(category, "text") == 0) lex_text_color = color;
   //else lex_term_color_help();
}



