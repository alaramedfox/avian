/* 
 *	Popcorn Kernel
 *	  File:		stdio.h
 *	  Purpose:	File and keyboard input and output handling
 */
 
#define STD_MAX	256			//Maximum characters for input 
#define ENTER_KEY_CODE 0x1C

#include "colordef.h"						//Color #DEFINEs and functions
#include "escapes.h"

extern "C" void __write_port(int16_t,byte);
 


/* void update_cursor(int row, int col)
 * by Dark Fiber
 */
void move_cursor(int row, int col)
{
	unsigned short position=(row*80) + col;

	// cursor LOW port to vga INDEX register
	__write_port(0x3D4, 0x0F);
	__write_port(0x3D5, (byte)(position&0xFF));
	// cursor HIGH port to vga INDEX register
	__write_port(0x3D4, 0x0E);
	__write_port(0x3D5, (byte)((position>>8)&0xFF));
}

namespace stdout 
{
	/* -- Standard Output -- */
	void scroll	(void);				//Scrolling screen support
	void write	(char);				//Safely write to MEMORY.IO.vram[MEMORY.INDEX.vram]
	void color	(color_t);			//Safely write to MEMORY.IO.vram[MEMORY.INDEX.vram+1]
	byte getcol	(void);				//Safely return the current column of the cursor
	byte getrow	(void);				//Safely return the current row of cursor
	void print	(string);			//Prints a string starting at cursor
	void printf	(color_t,string);	//Print with basic formatting
	void clear	(void);				//Clear the screen
	void move	(byte,byte);		//Move cursor with 2D
	void move	(int16_t);			//Move cursor with 1D
	
	void newline(void) 
	{
		move(THIS_ROW+1,0);
	}
	
	void creturn(void)
	{
		move(THIS_ROW+1,THIS_COL);
	}
	
	void setcolor_pair(const color_t fg, const color_t bg) 
	{
		MEMORY.GLOBAL.color = palette(fg,bg);
	}

	void setcolor(const color_t color) 
	{
		MEMORY.GLOBAL.color = color;
	}

	void move(byte row, byte col) 
	{
		if(row > VGA_R || col > VGA_C) return;
	
		MEMORY.INDEX.vram = row * VGA_C + col;
		if(MEMORY.INDEX.vram > VRAM_CHARS-1) { scroll(); }
	}
	
	void move(int16_t v) 
	{
		if(v >= VRAM_CHARS) {
			scroll(); 
			move(v-VGA_C); 
		}
		else {
			MEMORY.INDEX.vram = v; 
		}
	}
	
	void clear()
	{
		MEMORY.INDEX.vram = 0;
		setcolor(C_TERMINAL);
		for(count_t i=0; i<VRAM_CHARS; i++) {
			write('.');
			MEMORY.INDEX.vram++;
		}
		MEMORY.INDEX.vram = 0;
	}

	/* Safely write a single character to MEMORY.IO.vram without segfault */
	void write(char value) 
	{
		if(MEMORY.INDEX.vram > VRAM_CHARS-1) { 
			scroll(); 
			write(value); 
		}
		else { 
			MEMORY.IO.vram[MEMORY.INDEX.vram*2] = value; 
		}
	}

	void color(color_t value) 
	{
		if(MEMORY.INDEX.vram > VRAM_CHARS) { 
			scroll(); 
			color(value); 
		}
		else { 
			MEMORY.IO.vram[MEMORY.INDEX.vram*2+1] = value; 
		}
	}

	void scroll(void) 
	{
		/* Shift every row up one row */
		for(count_t i=0; i<=VRAM_LENGTH-VRAM_C; i++) {
			MEMORY.IO.vram[i] = MEMORY.IO.vram[i+VRAM_C];
		}
	
		move(23,0);
		for(count_t i=0;i<80; i++) {
			write(' ');
			color(MEMORY.GLOBAL.color);
			MEMORY.INDEX.vram++;
		}
		/* Set cursor to start of line */
		move(23,0);
	}

	void print(string str) 
	{
		/* Write the char* to VRAM starting at MEMORY.INDEX.vram */
		for(count_t i=0; i<str.size() /*&& str[i] != '\0'*/; i++) {
		
			/* Ignore (trailing) newline if kernel is listening to the keyboard */
			if(str[i] == '\n') {
				newline();
				MEMORY.INDEX.vram--; //Prevent skipping forward in index for these
			}
			else { 
				write(str[i]);
			}
		
			color(MEMORY.GLOBAL.color);
			MEMORY.INDEX.vram++;
		}
	}

	void printf(color_t color, string str) 
	{
		color_t old_color = MEMORY.GLOBAL.color;	//Remember previous color
		setcolor(color);									//Change to new color
		print(str);											//Call print()
		setcolor(old_color);								//Revert to the old color
	}

} // namespace stdout

namespace stdin
{
	void clear(void);								//Flush the input buffer
	void push(char);								//Push character to stdin
	char pop(void);								//Remove and return last char in stdin
	char peek(void);								//Return but do not remove last char
	string scan(void);
	
	string scan() 
	{
		MEMORY.FLAGS.listen = true;						//Set flag to listen for input
		MEMORY.FLAGS.repaint = true;					//Flag for repaint initially
		clear();												//Make sure stdin is empty before reading
		int16_t location = MEMORY.INDEX.vram;	//Remember starting vptr location
	
		while(peek() != '\n') {
			stdout::move(location);								//Move to initial location
		
			if(MEMORY.FLAGS.repaint)	{ //Only update screen if a key was pressed
				switch(MEMORY.GLOBAL.echostate) {
					case ECHO:		stdout::print(MEMORY.IO.instream);
										stdout::write(' ');
										break;
					case PASSWD: 	for(size_t i=0;i<MEMORY.IO.instream.size();i++)
							 			{ stdout::print("*"); } 							break;
					case NOECHO:	/* IMPLEMENT: Do not increment cursor */ 		break;
					default: 		/* print(MEMORY.IO.stdin); */						break;
				}
				MEMORY.FLAGS.repaint = false; 			//De-flag the update flag
				
				
			}
			move_cursor(THIS_ROW,THIS_COL+MEMORY.IO.instream.size()); //Print the blinking text cursor
			
		
		} /* Loop ends when user presses return */
		MEMORY.FLAGS.listen = false;					//De-flag input polling
		pop(); 												//Remove trailing newline
		string copy = string(MEMORY.IO.instream);
		clear();
		return copy;
	}

	void clear(void) 
	{
		MEMORY.FLAGS.repaint = true;	//Set repaint flag
		MEMORY.IO.instream.clear();	//Reset to empty string
	}

	void push(char c)
	{
		MEMORY.FLAGS.repaint = true;			//Set repaint flag
		if(MEMORY.IO.instream.size() < STD_MAX) {
			MEMORY.IO.instream.push(c);
		}
	}

	char pop(void) 
	{
		MEMORY.FLAGS.repaint = true;	//Flag for repaint
		if(MEMORY.IO.instream.size() > 0) {
			return MEMORY.IO.instream.pop();
		} else {
			clear();
			return '\0';
		}
	}

	char peek(void) 
	{
		if(MEMORY.IO.instream.size() > 0) {
			return MEMORY.IO.instream.peek();
		} else {
			return '\0';
		}
	}

} // namespace stdin
