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

extern const char HLINE1, HLINE2, HLINE3;
extern const char VLINE1, VLINE2;
extern const char BLOCK1, BLOCK2, BLOCK3, BLOCK4;

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
}

void newline(void)
{
	move(THIS_ROW+1,0);
}

inline void move(int row, int col)
{
	vptr = (col + VGA_C * row)-1;
	if(vptr >= VRAM_CHARS) { scroll(); }
}

inline void vmove(int16_t v)
{
	if(v >= VRAM_CHARS) { scroll(); vmove(v-VGA_C); }
	else { vptr = v; }
}

inline void setcolor_pair(color_t fg, color_t bg)
{
	this_color = palette(fg,bg);
}
inline void setcolor(color_t color)
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
		if(str[i] == '&')	//Handle escape codes
		{
			if(str[i+1] == 'h')
			{
				switch(str[i+2])
				{
					case '1': vram_char(HLINE1); break;
					case '2': vram_char(HLINE2); break;
					case '3': vram_char(HLINE3); break;
					default: break;
				}
				i++; //Skip forward to ignore the second escaped char
			}
			else if(str[i+1] == 'v')
			{
				switch(str[i+2])
				{
					case '1': vram_char(VLINE1); break;
					case '2': vram_char(VLINE2); break;
					default: break;
				}
				i++;
			}
			else if(str[i+1] == 'b')
			{
				switch(str[i+2])
				{
					case '1': vram_char(BLOCK1); break;
					case '2': vram_char(BLOCK2); break;
					case '3': vram_char(BLOCK3); break;
					case '4': vram_char(BLOCK4); break;
				}
				i++;
			}
			i++; //Skip forward to ignore the first escaped char
		} else { vram_char(str[i]); }
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

