#ifndef VGA_H_INCLUDED
#define VGA_H_INCLUDED
/*
 *		Popcorn Kernel - Bryan Webb
 *		File:		/include/vga.h
 *		Purpose:	Header for the video handler
 */
 
#include <types.h>
#include <color.h>
#include <asmfunc.h>

enum __VGA_LIMITS
{
	VGA_COL=80, VGA_ROW=24, VGA_LEN=1920,
	VGA_BYTES=3840,
};

typedef struct __VGA
{
	byte buffer[VGA_BYTES];
	size_t vptr;
	color_t color;
} vga_t;

void move_cursor(byte,byte);

/* Setters and getters */
size_t vga_getcol	(void);				//Safely return the current column of the cursor
size_t vga_getrow	(void);				//Safely return the current row of cursor
size_t vga_getloc (void);				//Safely return the 1D vptr location
void vga_setcolor (color_t);

/* Screen manipulation */
void vga_scroll	(void);				//Scrolling screen support
void vga_clear		(void);				//Clear the screen
void vga_movexy	(byte,byte);		//Move cursor with 2D
void vga_moveptr	(word);				//Move cursor with 1D
void vga_increment(void);				//Move to next cell
void vga_decrement(void);				//Move to previous cell

/* Printing */
void vga_write	(char);				//Safely write to vram[vptr]
void vga_color	(color_t);			//Safely write to vram[vptr+1]

/* Special characters */
void vga_newline(void);				//Move to next line, starting in first column
void vga_creturn(void);				//Move to next line, staying in same column

#endif