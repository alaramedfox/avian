#ifndef VGA_H_INCLUDED
#define VGA_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		VGA.h
 *			Purpose:	Video output handler
 */
 
/* Dependancies */
#include "colordef.h"
#include "keydef.h"
 
extern "C" void __write_port(int16_t,byte);

/* void update_cursor(int row, int col)
 * by Dark Fiber
 */
void move_cursor(int row, int col)
{
	unsigned short position=(row*80) + col;

	// cursor LOW port to VGA INDEX register
	__write_port(0x3D4, 0x0F);
	__write_port(0x3D5, (byte)(position&0xFF));
	// cursor HIGH port to VGA INDEX register
	__write_port(0x3D4, 0x0E);
	__write_port(0x3D5, (byte)((position>>8)&0xFF));
}
 
const struct bounds
{
	size_t size;
	size_t cols;
	size_t rows;
	
} B = {3840,160,48}, C = {1920,80,24};
 
class VGA
{
	private:
	byte *vram;
	byte VRAM_ALLOCATOR[4096]; //This is needed to allocate a block of memory for vram
	
	size_t vptr;
	color_t globalcolor;
	
	
	public:
	
	/* Constructors */
	VGA();
	
	/* Setters and getters */
	size_t getcol	(void);				//Safely return the current column of the cursor
	size_t getrow	(void);				//Safely return the current row of cursor
	size_t getloc 	(void);				//Safely return the 1D vptr location
	void setcolor  (color_t);
	void setcolor  (color_t, color_t);
	
	size_t video_size() { return sizeof(vram); }
	size_t video_addr() { return (size_t)&vram; }
	
	/* Screen manipulation */
	void scroll	(void);				//Scrolling screen support
	void clear	(void);				//Clear the screen
	void move	(byte,byte);		//Move cursor with 2D
	void move	(word);				//Move cursor with 1D

	
	/* Printing */
	void write	(char);				//Safely write to vram[vptr]
	void color	(color_t);			//Safely write to vram[vptr+1]
	void addch  (char);
	void print	(string);			//Prints a string starting at cursor
	void print_raw(string);
	void printf	(color_t,string);	//Print with basic formatting
	
	/* Special characters */
	void newline(void);				//Move to next line, starting in first column
	void creturn(void);				//Move to next line, staying in same column
	void tab(void);					//Insert 5 spaces

} stdout;

VGA::VGA()
{
	vram = (byte*)0xb8000;
	vptr = 0;
	globalcolor = 0x07;
}

size_t VGA::getcol() { return vptr % C.cols; }
size_t VGA::getrow() { return vptr / C.cols; }
size_t VGA::getloc() { return vptr; }
void	VGA::setcolor(color_t c) { globalcolor = c; }
void VGA::setcolor(color_t fg, color_t bg) { globalcolor = palette(fg,bg); }

void VGA::newline(void) { move(getrow()+1,0); }
void VGA::creturn(void)	{ move(getrow()+1,getcol()); }

void VGA::tab(void) 
{
	for(count_t i=0; i<MEMORY.GLOBAL.tabsize; i++) {
		addch(' ');
	}
	
}

void VGA::move(byte row, byte col) 
{
	if(row > C.rows || col > C.cols) {
		return;
	}
	else {
		move(row * C.cols + col);
	}
}

void VGA::move(word v) 
{
	if(v > C.size -1) {
		scroll();
	}
	else {
		vptr = v; 
	}
}

void VGA::clear()
{
	vptr = 0;
	setcolor(C_TERMINAL);
	for(count_t i=0; i<C.size; i++) {
		write(' ');
		vptr++;
	}
	vptr = 0;
}

/* Safely write a single character to vram without segfault */
void VGA::write(char value) 
{
	vram[vptr*2] = value; 
}

void VGA::addch(char value)
{
	write(value);
	color(globalcolor);
	vptr++;
}

void VGA::color(color_t value) 
{
	vram[vptr*2+1] = value; 
}

void VGA::scroll(void) 
{
	/* Shift every row up one row */
	for(count_t i=0; i<=B.size - B.cols; i++) {
		vram[i] = vram[i+B.cols];
	}

	/* Draw empty row at the bottom */
	move(23,0);
	for(count_t i=0; i<C.cols; i++) {
		write(' ');
		color(globalcolor);
		vptr++;
	}
	
	move(23,0);
}

void VGA::print(string str) 
{
	/* Write the char* to VRAM starting at MEMORY.INDEX.vram */
	for(count_t i=0; i<str.size(); i++) {
	
		     if(str[i] == '\n') { newline(); }
		else if(str[i] == '\0') { ; }
		else if(str[i] == '\r') { creturn(); }
		else if(str[i] == '\t') { tab(); }
		else {
			addch(str[i]);
		}
	}
}

void VGA::print_raw(string str)
{
	for(count_t i=0; i<str.size(); i++) {
		switch(str[i]) {
			case '\n': print("\\n"); break;
			case '\0': print("\\0"); break;
			case '\r': print("\\r"); break;
			case '\t': print("\\t"); break;
			default: addch(str[i]); break;
		}
	}
}
#endif