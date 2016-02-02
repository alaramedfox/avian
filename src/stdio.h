/* 
 *	Popcorn Kernel
 *	  File:		stdio.h
 *	  Purpose:	File and keyboard input and output handling
 */
 
#define STD_MAX	256			//Maximum characters for input 
#define ENTER_KEY_CODE 0x1C

#include "colordef.h"						//Color #DEFINEs and functions
 
extern const char HLINE1, HLINE2, HLINE3,
						VLINE1, VLINE2,
						BLOCK1, BLOCK2, BLOCK3, BLOCK4;

void vmove(int16_t);							//Safely move the MEMORY.IO.stdout pointer.
void print(const char str[]);				//Prints a string starting at vpt
char* scan(void);								//Reads and returns stdin content
void clear(void);								//Clears the screen
void move(int8_t,int8_t);					//Moves to 2D location in VRAM
void newline(void);							//Increment to start of next row
void setcolor_pair(int8_t,int8_t);		//Change the screen color: pairs
void setcolor(int8_t);						//Change the screen color: palette

/* -- Standard Output -- */
void stdout_scroll(void);					//Scrolling screen support
void stdout_char(char);						//Safely write to MEMORY.IO.stdout[MEMORY.INDEX.stdout]
void stdout_color(int8_t);					//Safely write to MEMORY.IO.stdout[MEMORY.INDEX.stdout+1]
int16_t stdout_column(void);				//Safely return the current column of the cursor
int16_t stdout_row(void);					//Safely return the current row of cursor

/* -- Standard Input --  */
void stdin_clear(void);						//Flush the input buffer
void stdin_push(char);						//Push character to stdin
char stdin_pop(void);						//Remove and return last char in stdin
char stdin_peek(void);						//Return but do not remove last char



void clear(void) {
	MEMORY.INDEX.stdout = 0;
	setcolor(C_TERMINAL);
	for(int i=0; i<VRAM_CHARS; i++) {
		stdout_char(' ');
		stdout_color(C_TERMINAL);
		MEMORY.INDEX.stdout++;
	}
	MEMORY.INDEX.stdout = 0;
}

void newline(void) {
	move(THIS_ROW+1,0);
}

void move(const int8_t row, const int8_t col) {
	if(row > VGA_R || col > VGA_C) return;
	
	MEMORY.INDEX.stdout = row * VGA_C + col;
	if(MEMORY.INDEX.stdout > VRAM_CHARS-1) { stdout_scroll(); }
}

void vmove(int16_t v) {
	if(v >= VRAM_CHARS) {
		stdout_scroll(); 
		vmove(v-VGA_C); 
	}
	else {
		MEMORY.INDEX.stdout = v; 
	}
}

void setcolor_pair(const int8_t fg, const int8_t bg) {
	MEMORY.GLOBAL.color = palette(fg,bg);
}

void setcolor(const int8_t color) {
	MEMORY.GLOBAL.color = color;
}

/* Safely write to MEMORY.IO.stdout without segfault */
void stdout_char(const char value) {
	if(MEMORY.INDEX.stdout >= VRAM_CHARS) { 
		stdout_scroll(); 
		stdout_char(value); 
	}
	else { 
		MEMORY.IO.stdout[MEMORY.INDEX.stdout*2] = value; 
	}
}

void stdout_color(const int8_t value) {
	if(MEMORY.INDEX.stdout >= VRAM_CHARS) { 
		stdout_scroll(); 
		stdout_color(value); 
	}
	else { 
		MEMORY.IO.stdout[MEMORY.INDEX.stdout*2+1] = value; 
	}
}

void stdout_scroll(void) {
	/* Shift every row up one row */
	for(int i=0; i<=VRAM_LENGTH-VRAM_C; i++) {
		MEMORY.IO.stdout[i] = MEMORY.IO.stdout[i+VRAM_C];
	}
	
	move(23,0);
	for(int i=0;i<80; i++) {
		stdout_char(' ');
		stdout_color(MEMORY.GLOBAL.color);
		MEMORY.INDEX.stdout++;
	}
	/* Set cursor to start of line */
	move(23,0);
}

void print(const char str[]) 
{
	/* Write the string to VRAM starting at MEMORY.INDEX.stdout */
	for(int i=0; str[i] != '\0'; i++) {
		
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
					case '1': stdout_char(HLINE1); break;
					case '2': stdout_char(HLINE2); break;
					case '3': stdout_char(HLINE3); break;
					default: break;
				}
				++i; //Skip forward to ignore the second escaped char
			}
			else if(str[i+1] == 'v') {
				switch(str[i+2]) {
					case '1': stdout_char(VLINE1); break;
					case '2': stdout_char(VLINE2); break;
					default: break;
				}
				++i;
			}
			else if(str[i+1] == 'b') {
				switch(str[i+2]) {
					case '1': stdout_char(BLOCK1); break;
					case '2': stdout_char(BLOCK2); break;
					case '3': stdout_char(BLOCK3); break;
					case '4': stdout_char(BLOCK4); break;
				}
				++i;
			}
			++i; //Skip forward to ignore the first escaped char
		} else { 
			stdout_char(str[i]);
		}
		
		stdout_color(MEMORY.GLOBAL.color);
		MEMORY.INDEX.stdout++;
	}
	return;
}

void printf(int8_t color, const char str[]) {
	int8_t old_color = MEMORY.GLOBAL.color;	//Remember previous color
	setcolor(color);									//Change to new color
	print(str);											//Call print()
	setcolor(old_color);								//Revert to the old color
}

char* scan(void) {
	MEMORY.FLAGS.stdin = true;						//Set flag to listen for input
	MEMORY.FLAGS.stdout = true;					//Flag for repaint initially
	stdin_clear();										//Make sure stdin is empty before reading
	int16_t location = MEMORY.INDEX.stdout;	//Remember starting vptr location
	
	while(stdin_peek() != '\n') {
		vmove(location);								//Move to initial location
		
		if(MEMORY.FLAGS.stdout)	{ //Only update screen if a key was pressed
			switch(MEMORY.GLOBAL.echostate) {
				case ECHO: 		print(MEMORY.IO.stdin); print("     ");		break;
				case PASSWD: 	for(size_t i=0;i<MEMORY.INDEX.stdin;i++)
					    			{ print("*"); } 										break;
				case NOECHO:	/* IMPLEMENT: Do not increment cursor */ 		break;
				default: 		/* print(MEMORY.IO.stdin); */						break;
			}
			MEMORY.FLAGS.stdout = false; 			//De-flag the update flag
		}
		vmove(location + MEMORY.INDEX.stdin); 	//Move to current cursor
		printf(C_BLINK," ");							//Print the blinking text cursor
		
	} /* Loop ends when user presses return */
	MEMORY.FLAGS.stdin = false;					//De-flag input polling
	stdin_pop(); 										//Remove trailing newline
	vmove(location + MEMORY.INDEX.stdin);		//Move to CURRENT cursor
	print(" "); 										//erase leftover blinky thing
	return MEMORY.IO.stdin;
}

void stdin_clear(void) {
	MEMORY.FLAGS.stdout = true;					//Set repaint flag
	MEMORY.IO.stdin[0] = '\0';						//Set first char in stdout to null
	MEMORY.INDEX.stdin  = 0;						//Move pointer to start of array
}

void stdin_push(char c) {
	MEMORY.FLAGS.stdout = true;					//Set repaint flag
	if(MEMORY.INDEX.stdin < STD_MAX) {
		MEMORY.IO.stdin[MEMORY.INDEX.stdin] = c;
		MEMORY.IO.stdin[MEMORY.INDEX.stdin+1] = '\0';
		MEMORY.INDEX.stdin++;
	} 
	else {
		stdin_push(STD_MAX-1);
	}
}

char stdin_pop(void) {
	MEMORY.FLAGS.stdout = true;	//Flag for repaint
	if(MEMORY.INDEX.stdin > 0) {
		MEMORY.INDEX.stdin--;
		MEMORY.IO.stdin[MEMORY.INDEX.stdin] = '\0';
		return MEMORY.IO.stdin[MEMORY.INDEX.stdin-1];
	} else {
		stdin_clear();
		return '\0';
	}
}

char stdin_peek(void) {
	if(MEMORY.INDEX.stdin > 0) {
		return MEMORY.IO.stdin[MEMORY.INDEX.stdin-1];
	} else {
		return '\0';
	}
}
