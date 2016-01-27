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

#define THIS_COL	vptr%(VGA_C)		//Current column in VRAM
#define THIS_ROW	vptr/(VGA_C)		//Current row in VRAM

char *vram = (char*)VRAM_START;		//Access to system video RAM
int16_t vptr = 0;					//pointer to current vram spot
color_t this_color = 0x07;			//Register for current color

bool VUPDATE=false;					//Flag to update something

void vmove(int16_t);				//Safely move the vram pointer.

void print(const char str[]);			//Prints a string starting at vpt
void clear(void);					//Clears the screen
void move(int,int);					//Moves to 2D location in VRAM
void newline(void);					//Increment to start of next row
void setcolor_pair(color_t,color_t);	//Change the screen color: pairs
void setcolor(color_t);				//Change the screen color: palette
void scroll(void);					//Scrolling screen support
void vram_char(char);				//Safely write to vram[vptr]
void vram_color(color_t);			//Safely write to vram[vptr+1]


void clear(void)
{
	for(int i=0; i<VRAM_CHARS; i++)	//Loop 
	{
		vptr = i;
		vram_char(' ');
		vram_color(palette(C_WHITE,C_BLACK));
	}
	
	
	vptr = 0;
	return;
}

void move(int row, int col)
{
	vptr = (col + VGA_C * row)-1;
	if(vptr >= VRAM_CHARS) { scroll(); }
}

void vmove(int16_t v)
{
	if(v >= VRAM_CHARS) { scroll(); vmove(v-VGA_C); }
	else { vptr = v; }
}

void setcolor_pair(color_t fg, color_t bg)
{
	this_color = palette(fg,bg);
}
void setcolor(color_t color)
{
	this_color = color;
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
		vptr++;
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
void printf(color_t color, const char str[])
{
	color_t old_color = this_color;
	setcolor(color);
	print(str);
	setcolor(old_color);
}

