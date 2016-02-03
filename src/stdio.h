/* 
 *	Popcorn Kernel
 *	  File:		stdio.h
 *	  Purpose:	File and keyboard input and output handling
 */
 
#define STD_MAX	256			//Maximum characters for input 
#define ENTER_KEY_CODE 0x1C

#include "colordef.h"						//Color #DEFINEs and functions

extern "C" void __write_port(int16_t,byte);
 
extern const char HLINE1, HLINE2, HLINE3,
						VLINE1, VLINE2,
						BLOCK1, BLOCK2, BLOCK3, BLOCK4;

void vmove(int16_t);							//Safely move the MEMORY.IO.stdout pointer.

string scan(void);								//Reads and returns stdin content
void clear(void);								//Clears the screen
void move(byte,byte);					//Moves to 2D location in VRAM
void newline(void);							//Increment to start of next row
void setcolor_pair(color_t,color_t);		//Change the screen color: pairs
void setcolor(color_t);						//Change the screen color: palette



/* -- Standard Input --  */
void stdin_clear(void);						//Flush the input buffer
void stdin_push(char);						//Push character to stdin
char stdin_pop(void);						//Remove and return last char in stdin
char stdin_peek(void);						//Return but do not remove last char

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







void setcolor_pair(const color_t fg, const color_t bg) {
	MEMORY.GLOBAL.color = palette(fg,bg);
}

void setcolor(const color_t color) {
	MEMORY.GLOBAL.color = color;
}

namespace stdout 
{
	/* -- Standard Output -- */
	void scroll	(void);				//Scrolling screen support
	void write	(char);				//Safely write to MEMORY.IO.stdout[MEMORY.INDEX.stdout]
	void color	(color_t);			//Safely write to MEMORY.IO.stdout[MEMORY.INDEX.stdout+1]
	byte getcol	(void);				//Safely return the current column of the cursor
	byte getrow	(void);				//Safely return the current row of cursor
	void print	(string);			//Prints a string starting at cursor
	void printf	(color_t,string);	//Print with basic formatting
	void clear	(void);				//Clear the screen
	
	void newline(void) 
	{
		move(THIS_ROW+1,0);
	}

	void move(byte row, byte col) 
	{
		if(row > VGA_R || col > VGA_C) return;
	
		MEMORY.INDEX.stdout = row * VGA_C + col;
		if(MEMORY.INDEX.stdout > VRAM_CHARS-1) { scroll(); }
	}
	
	void move(int16_t v) 
	{
		if(v >= VRAM_CHARS) {
			scroll(); 
			move(v-VGA_C); 
		}
		else {
			MEMORY.INDEX.stdout = v; 
		}
	}
	
	void clear(void)
	{
		MEMORY.INDEX.stdout = 0;
		setcolor(C_TERMINAL);
		for(int i=0; i<VRAM_CHARS; i++) {
			write(' ');
			write(C_TERMINAL);
			MEMORY.INDEX.stdout++;
		}
		MEMORY.INDEX.stdout = 0;
	}

	/* Safely write a single character to MEMORY.IO.stdout without segfault */
	void write(char value) 
	{
		if(MEMORY.INDEX.stdout >= VRAM_CHARS) { 
			scroll(); 
			write(value); 
		}
		else { 
			MEMORY.IO.stdout[MEMORY.INDEX.stdout*2] = value; 
		}
	}

	void color(color_t value) 
	{
		if(MEMORY.INDEX.stdout >= VRAM_CHARS) { 
			scroll(); 
			color(value); 
		}
		else { 
			MEMORY.IO.stdout[MEMORY.INDEX.stdout*2+1] = value; 
		}
	}

	void scroll(void) 
	{
		/* Shift every row up one row */
		for(int i=0; i<=VRAM_LENGTH-VRAM_C; i++) {
			MEMORY.IO.stdout[i] = MEMORY.IO.stdout[i+VRAM_C];
		}
	
		move(23,0);
		for(int i=0;i<80; i++) {
			write(' ');
			color(MEMORY.GLOBAL.color);
			MEMORY.INDEX.stdout++;
		}
		/* Set cursor to start of line */
		move(23,0);
	}

	void print(string str) 
	{
		/* Write the char* to VRAM starting at MEMORY.INDEX.stdout */
		for(int i=0; i<str.size(); i++) {
		
			/* Ignore (trailing) newline if kernel is listening to the keyboard */
			if(str[i] == '\n') {
				newline();
				MEMORY.INDEX.stdout--; //Prevent skipping forward in index for these
			}
			else if(str[i] == '\t') {
				/* Don't know how to handle tabs yet */
			}
			else if(str[i] == '&') {
				if(str[i+1] == 'h') {
					switch(str[i+2]) {
						case '1': write(HLINE1); break;
						case '2': write(HLINE2); break;
						case '3': write(HLINE3); break;
						default: break;
					}
					++i; //Skip forward to ignore the second escaped char
				}
				else if(str[i+1] == 'v') {
					switch(str[i+2]) {
						case '1': write(VLINE1); break;
						case '2': write(VLINE2); break;
						default: break;
					}
					++i;
				}
				else if(str[i+1] == 'b') {
					switch(str[i+2]) {
						case '1': write(BLOCK1); break;
						case '2': write(BLOCK2); break;
						case '3': write(BLOCK3); break;
						case '4': write(BLOCK4); break;
					}
					++i;
				}
				++i; //Skip forward to ignore the first escaped char
			} else { 
				write(str[i]);
			}
		
			color(MEMORY.GLOBAL.color);
			MEMORY.INDEX.stdout++;
		}
	} /* void print(string str); */

	void printf(color_t color, string str) 
	{
		color_t old_color = MEMORY.GLOBAL.color;	//Remember previous color
		setcolor(color);									//Change to new color
		print(str);											//Call print()
		setcolor(old_color);								//Revert to the old color
	}

} // namespace stdout

string scan(void) 
{
	MEMORY.FLAGS.stdin = true;						//Set flag to listen for input
	MEMORY.FLAGS.stdout = true;					//Flag for repaint initially
	stdout::clear();										//Make sure stdin is empty before reading
	int16_t location = MEMORY.INDEX.stdout;	//Remember starting vptr location
	
	while(stdin_peek() != '\n') {
		stdout::move(location);								//Move to initial location
		
		if(MEMORY.FLAGS.stdout)	{ //Only update screen if a key was pressed
			switch(MEMORY.GLOBAL.echostate) {
				case ECHO: 		stdout::print(MEMORY.IO.stdin); 
									stdout::print(" ");
									break;
				case PASSWD: 	for(size_t i=0;i<MEMORY.INDEX.stdin;i++)
					    			{ stdout::print("*"); } 							break;
				case NOECHO:	/* IMPLEMENT: Do not increment cursor */ 		break;
				default: 		/* print(MEMORY.IO.stdin); */						break;
			}
			MEMORY.FLAGS.stdout = false; 			//De-flag the update flag
		}
		move_cursor(THIS_ROW,THIS_COL+MEMORY.INDEX.stdin); //Print the blinking text cursor
		
	} /* Loop ends when user presses return */
	MEMORY.FLAGS.stdin = false;					//De-flag input polling
	stdin_pop(); 										//Remove trailing newline
	//vmove(location + MEMORY.INDEX.stdin);		//Move to CURRENT cursor
	//print(" "); 										//erase leftover blinky thing
	string copy;
	for(int i=0; MEMORY.IO.stdin[i] != '\0'; i++) { 
		copy.push(MEMORY.IO.stdin[i]); 
	}
	stdin_clear();
	return copy;
}

void stdin_clear(void) {
	MEMORY.FLAGS.stdout = true;					//Set repaint flag
	MEMORY.IO.stdin = string(); /*
	MEMORY.IO.stdin[0] = '\0';						//Set first char in stdout to null
	MEMORY.INDEX.stdin  = 0;						//Move pointer to start of array
	*/
}

void stdin_push(char c) {
	MEMORY.FLAGS.stdout = true;					//Set repaint flag
	if(MEMORY.INDEX.stdin < STD_MAX) {
		MEMORY.IO.stdin.push(c);
	} 
	else {
		stdin_push(STD_MAX-2);
	}
}

char stdin_pop(void) {
	MEMORY.FLAGS.stdout = true;	//Flag for repaint
	if(MEMORY.INDEX.stdin > 0) {
		return MEMORY.IO.stdin.pop();
	} else {
		stdin_clear();
		return '\0';
	}
}

char stdin_peek(void) {
	if(MEMORY.INDEX.stdin > 0) {
		return MEMORY.IO.stdin[MEMORY.INDEX.stdin-1];
	} else {
		MEMORY.INDEX.stdin = 0;
		return '\0';
	}
}
