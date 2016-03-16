#ifndef VGA_H_INCLUDED
#define VGA_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:          avian/drivers/vga.h
//      Purpose:       Header for the VGA driver
// ======================================================================== */
 
#include <stdlib.h>
#include <asmfunc.h>
#include <string.h>

enum __VGA_LIMITS
{
   VGA_COL=80, VGA_ROW=25, VGA_LEN=2000,
   VGA_BYTES=4000,
};

enum __COLOR_DEFS
{
   BLACK=0x0, BLUE=0x1, GREEN=0x2, CYAN=0x3,
   RED=0x4, MAGENTA=0x5, BROWN=0x6, GREY=0x7,
   BOLD=1, NORMAL=0,
   
   C_TERMINAL=0x07, C_BLUESCR=0x1F, 
   C_WARN=0x04, C_CRITICAL=0x47,
};

typedef struct __VGA
{
   byte buffer[VGA_BYTES];
   size_t vptr;
   byte color;
   
} FLAT vga_t;

void move_cursor(byte,byte);
void hide_cursor(void);

/* Setters and getters */
size_t vga_getcol   (void);          //Safely return the current column of the cursor
size_t vga_getrow   (void);          //Safely return the current row of cursor
size_t vga_getloc (void);            //Safely return the 1D vptr location
void   vga_setcolor (byte);
int    vga_tabsize  (int);

/* Screen manipulation */
void vga_scroll   (void);            //Scrolling screen support
void vga_clear      (void);          //Clear the screen
void vga_clear_row(byte);            //Clears a given row
void vga_movexy   (byte,byte);       //Move cursor with 2D
void vga_moveptr   (word);           //Move cursor with 1D
void vga_increment(void);            //Move to next cell
void vga_decrement(void);            //Move to previous cell
char vga_char_at  (byte,byte);       //Return character at 2D point

/* Printing */
void vga_write   (char);             //Safely write to vram[vptr]
void vga_color   (byte);          //Safely write to vram[vptr+1]

void addch(char);
void print(const char[]);
void println(const char[]);
void iprint(dword, byte);
void printxy(byte,byte,const char[]);

/* Special characters */
void vga_newline(void);            //Move to next line, starting in first column
void vga_creturn(void);            //Move to next line, staying in same column
void vga_tabchar(void);            //Print 4 spaces

#endif