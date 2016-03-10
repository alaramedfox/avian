#ifndef VGA_H_INCLUDED
#define VGA_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:          avian/drivers/vga.h
//      Purpose:       Header for the VGA driver
// ======================================================================== */
 
#include <stdlib.h>
#include <color.h>
#include <asmfunc.h>
#include <string.h>

enum __VGA_LIMITS
{
   VGA_COL=80, VGA_ROW=25, VGA_LEN=2000,
   VGA_BYTES=4000,
};

typedef struct __VGA
{
   byte buffer[VGA_BYTES];
   size_t vptr;
   color_t color;
   
} FLAT vga_t;

void move_cursor(byte,byte);
void hide_cursor(void);

/* Setters and getters */
size_t vga_getcol   (void);          //Safely return the current column of the cursor
size_t vga_getrow   (void);          //Safely return the current row of cursor
size_t vga_getloc (void);            //Safely return the 1D vptr location
void   vga_setcolor (color_t);

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
void vga_color   (color_t);          //Safely write to vram[vptr+1]

__attribute__((hot)) void addch(char);
__attribute__((hot)) void print(const char[]);
__attribute__((hot)) void println(const char[]);
__attribute__((hot)) void iprint(dword, byte);

/* Special characters */
void vga_newline(void);            //Move to next line, starting in first column
void vga_creturn(void);            //Move to next line, staying in same column
void vga_tabchar(void);            //Print 4 spaces

#endif