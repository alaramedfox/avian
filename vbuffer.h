/* 	
 *	Popcorn Kernel
 *	  File:   	vbuffer.h
 *	  Purpose:	Handle the terminal screen buffer
 */
 
#include "colordef.h"				//Color #DEFINEs and functions
 
#define VGACOLS	80				//Define the number of char columns
#define VGAROWS	24				//Define the number of char rows
#define VGA_W		80*2				//Width of VGA in bytes
#define VGA_H		24*2				//Height of VGA in bytes

#define VRAM_START	0xb8000			//Actual address of system VRAM
#define VRAM_LENGTH	80*24*2			//Length of VRAM in bytes
#define VRAM_CHARS	VRAM_LENGTH/2		//Length of VRAM in words

#define CHAR_NEWLINE 0xA				//Define the newline char '\n'
#define CHAR_TAB	 0x9				//Define the tab char '\t'

#define THIS_COL	vptr%(VGA_H)		//Current column in VRAM
#define THIS_ROW	vptr/(VGA_W)		//Current row in VRAM

char *vram = (char*)VRAM_START;		//Access to system video RAM
unsigned int vptr = 0;				//pointer to current vram spot
color_t this_color = 0x07;			//Register for current color

void move_vptr(unsigned int);			//Safely move the vram pointer.

void print(const char *str);			//Prints a string starting at vpt
void clear(void);					//Clears the screen
void move(int,int);					//Moves to 2D location in VRAM
void newline(void);					//Increment to start of next row
void setcolor(color_t,color_t);		//Change the screen color
void scroll(void);					//Scrolling screen support
void vram_this(char);				//Safely write to vram[vptr]
void vram_next(color_t);				//Safely write to vram[vptr+1]
void vram_over();					//Iterate to next char in vram
void vram_over(){vptr+=2;}
void deselect();					//Un-Highlight old cursor location
void select();						//Highlight current cursor location

void clear(void)
{
	for(int i=0; i<VRAM_LENGTH; i+=2)	//Loop 
	{
		vptr = i;
		vram_this(' ');
		vram_next(palette(C_WHITE,C_BLACK));
	}
	
	/* Reset video pointer to 0 */
	vptr = 0;
	return;
}

void move(int row, int col)
{
	vptr = 2*(col + VGACOLS * row)-2;
	if(vptr >= VRAM_LENGTH-1) { scroll(); }
}

void setcolor(color_t fg, color_t bg)
{
	this_color = palette(fg,bg);
}

/* Safely write to vram without segfault */
void vram_this(char value)
{
	if(vptr >= VRAM_LENGTH-1) { scroll(); }
	else { vram[vptr] = value; }
}

void vram_next(color_t value)
{
	if(vptr+1 >= VRAM_LENGTH) { scroll();  }
	else { vram[vptr+1] = value; }
}

void scroll(void)
{
	/* Shift every row up one row */
	for(int i=VGA_W; i<VRAM_LENGTH; i++)
	{
		vram[i-VGA_W] = vram[i];
	}
	
	move(23,0);
	for(int i=0;i<80; i++)
	{
		vram_over();
		vram_this(' ');
		vram_next(this_color);
	}
	/* Set cursor to start of line */
	move(23,0);
}

void print(const char *str)
{
	/* Write the string to VRAM starting at vptr */
	for(int i=0; str[i] != '\0'; i++)
	{
		vram_over();
		vram_this(str[i]);
		vram_next(this_color);
	}
	return;
}

