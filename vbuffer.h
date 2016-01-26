/* 	
 *	Popcorn Kernel
 *	  File:   	vbuffer.h
 *	  Purpose:	Handle the terminal screen buffer
 */
 
#include "colordef.h"				//Color #DEFINEs and functions
 
#define VRAM_C	80*2				//Define the number of char columns
#define VRAM_R	24*2				//Define the number of char rows
#define VGA_C	80				//Width of VGA in bytes
#define VGA_R	24				//Height of VGA in bytes

#define VRAM_START	0xb8000			//Actual address of system VRAM
#define VRAM_LENGTH	80*24*2			//Length of VRAM in bytes
#define VRAM_CHARS	VRAM_LENGTH/2		//Length of VRAM in words

#define CHAR_NEWLINE 0xA				//Define the newline char '\n'
#define CHAR_TAB	 0x9				//Define the tab char '\t'

#define THIS_COL	vptr%(VRAM_C)		//Current column in VRAM
#define THIS_ROW	vptr/(VRAM_C)		//Current row in VRAM

char *vram = (char*)VRAM_START;		//Access to system video RAM
int16_t vptr = 0;					//pointer to current vram spot
color_t this_color = 0x07;			//Register for current color

void move_vptr(unsigned int);			//Safely move the vram pointer.

void print(const char *str);			//Prints a string starting at vpt
void clear(void);					//Clears the screen
void move(int,int);					//Moves to 2D location in VRAM
void newline(void);					//Increment to start of next row
void setcolor(color_t,color_t);		//Change the screen color
void scroll(void);					//Scrolling screen support
void vram_char(char);				//Safely write to vram[vptr]
void vram_color(color_t);				//Safely write to vram[vptr+1]
void vram_over();					//Iterate to next char in vram
void vram_over(){vptr+=2;}

void clear(void)
{
	for(int i=0; i<VRAM_CHARS; i++)	//Loop 
	{
		vptr = i;
		vram_char(' ');
		vram_color(palette(C_WHITE,C_BLACK));
	}
	
	/* Reset video pointer to 0 */
	vptr = 0;
	return;
}

void move(int row, int col)
{
	vptr = (col + VGA_C * row)-2;
	if(vptr >= VRAM_CHARS) { scroll(); }
}
void vmove(int v)
{
	if(v >= VRAM_LENGTH) { scroll(); vptr = v - VRAM_C; }
	else { vptr = v; }
}

void setcolor(color_t fg, color_t bg)
{
	this_color = palette(fg,bg);
}

/* Safely write to vram without segfault */
void vram_char(char value)
{
	if(vptr >= VRAM_CHARS) { scroll(); }
	else { vram[vptr*2] = value; }
}

void vram_color(color_t value)
{
	if(vptr >= VRAM_CHARS) { scroll();  }
	else { vram[vptr*2+1] = value; }
}

void scroll(void)
{
	/* Shift every row up one row */
	for(int i=VRAM_C; i<VRAM_CHARS; i++)
	{
		vram[(i-VGA_C)*2] = vram[i*2];
	}
	
	move(23,0);
	for(int i=0;i<80; i++)
	{
		vram_over();
		vram_char(' ');
		vram_color(this_color);
	}
	/* Set cursor to start of line */
	move(23,0);
}

void print(const char str[])
{
	/* Write the string to VRAM starting at vptr */
	for(int i=0; str[i] != '\0'; i++)
	{
		vptr++;
		vram_char(str[i]);
		vram_color(this_color);
	}
	return;
}

