/* 
 *	Popcorn Kernel
 *	  File:		stdio.h
 *	  Purpose:	File and keyboard input and output handling
 */
 
#define STD_MAX	256			//Maximum characters for input 
#define ENTER_KEY_CODE 0x1C

#define	NOECHO	1
#define	ECHO		2
#define	PASSWD	3

#include "colordef.h"						//Color #DEFINEs and functions
 


extern const char HLINE1, HLINE2, HLINE3,
						VLINE1, VLINE2,
						BLOCK1, BLOCK2, BLOCK3, BLOCK4;

void vmove(int16_t);							//Safely move the MEMORY.STDIO.stdout pointer.
void print(const char str[]);				//Prints a string starting at vpt
void clear(void);								//Clears the screen
void move(int,int);							//Moves to 2D location in VRAM
void newline(void);							//Increment to start of next row
void setcolor_pair(color_t,color_t);	//Change the screen color: pairs
void setcolor(color_t);						//Change the screen color: palette
void scroll(void);							//Scrolling screen support
void vram_char(char);						//Safely write to MEMORY.STDIO.stdout[MEMORY.INDEX.stdout]
void vram_color(color_t);					//Safely write to MEMORY.STDIO.stdout[MEMORY.INDEX.stdout+1]
	
void stdin_clear();							//Flush the input buffer
void stdin_push(char);						//Push character to stdin
char stdin_pop();								//Remove and return last char in stdin
char stdin_peek();							//Return but do not remove last char

void clear(void) {
	for(int i=0; i<VRAM_CHARS; i++) {
		MEMORY.INDEX.stdout = i;
		vram_char(' ');
		vram_color(palette(C_WHITE,C_BLACK));
	}
	MEMORY.INDEX.stdout = 0;
}

inline void newline(void) {
	move(THIS_ROW+1,0);
}

inline void move(int row, int col) {
	MEMORY.INDEX.stdout = (col + VGA_C * row)-1;
	if(MEMORY.INDEX.stdout >= VRAM_CHARS) { scroll(); }
}

inline void vmove(int16_t v) {
	if(v >= VRAM_CHARS) { scroll(); vmove(v-VGA_C); }
	else { MEMORY.INDEX.stdout = v; }
}

inline void setcolor_pair(color_t fg, color_t bg) {
	MEMORY.GLOBAL.color = palette(fg,bg);
}
inline void setcolor(color_t color) {
	MEMORY.GLOBAL.color = color;
}

/* Safely write to MEMORY.STDIO.stdout without segfault */
void vram_char(char value) {
	if(MEMORY.INDEX.stdout >= VRAM_CHARS) { scroll(); vram_char(value); }
	else { MEMORY.STDIO.stdout[MEMORY.INDEX.stdout*2] = value; }
}

void vram_color(color_t value) {
	if(MEMORY.INDEX.stdout >= VRAM_CHARS) { scroll(); vram_color(value); }
	else { MEMORY.STDIO.stdout[MEMORY.INDEX.stdout*2+1] = value; }
}

void scroll(void) {
	/* Shift every row up one row */
	for(int i=0; i<=VRAM_LENGTH-VRAM_C; i++) {
		MEMORY.STDIO.stdout[i] = MEMORY.STDIO.stdout[i+VRAM_C];
	}
	
	move(23,0);
	for(int i=0;i<80; i++) {
		MEMORY.INDEX.stdout++;
		vram_char(' ');
		vram_color(MEMORY.GLOBAL.color);
	}
	/* Set cursor to start of line */
	move(23,1);
}

void print(const char str[]) {
	/* Write the string to VRAM starting at MEMORY.INDEX.stdout */
	for(int i=0; str[i] != '\0'; i++) {
		MEMORY.INDEX.stdout++;
		
		if(str[i] == '\n') {
			newline();
		}
		else if(str[i] == '\t') {
			tab();
		}
		else if(str[i] == '&') {
			if(str[i+1] == 'h') {
				switch(str[i+2]) {
					case '1': vram_char(HLINE1); break;
					case '2': vram_char(HLINE2); break;
					case '3': vram_char(HLINE3); break;
					default: break;
				}
				++i; //Skip forward to ignore the second escaped char
			}
			else if(str[i+1] == 'v') {
				switch(str[i+2]) {
					case '1': vram_char(VLINE1); break;
					case '2': vram_char(VLINE2); break;
					default: break;
				}
				++i;
			}
			else if(str[i+1] == 'b') {
				switch(str[i+2]) {
					case '1': vram_char(BLOCK1); break;
					case '2': vram_char(BLOCK2); break;
					case '3': vram_char(BLOCK3); break;
					case '4': vram_char(BLOCK4); break;
				}
				++i;
			}
			++i; //Skip forward to ignore the first escaped char
		} else { 
			vram_char(str[i]);
		}
		vram_color(MEMORY.GLOBAL.color);
	}
	return;
}

void printf(color_t color, const char str[]) {
	color_t old_color = MEMORY.GLOBAL.color;
	setcolor(color);
	print(str);
	setcolor(old_color);
}

char* stdin(int8_t FLAG) {
	MEMORY.FLAGS.stdout = true;	//Flag for repaint initially
	stdin_clear();
	int16_t location = MEMORY.INDEX.stdout;
	int32_t counter = 0;
	bool blink = true;
	while(stdin_peek() != '\n') {
		vmove(location);
		
		if(MEMORY.FLAGS.stdout)	{ //Only update screen if a key was pressed
			switch(FLAG) {
				case ECHO: 		print(MEMORY.STDIO.stdout); 						break;
				case PASSWD: 	for(size_t i=0;i<MEMORY.INDEX.stdin;i++)
					    			{ print("*"); } 							break;
				case NOECHO:													break;
				default: print(MEMORY.STDIO.stdout); 								break;
			}
			MEMORY.FLAGS.stdout = false; //De-flag the update flag
		}
		vmove(location+MEMORY.INDEX.stdin);
		blink?print("_ "):print("  ");
		if(counter > 65534*3) { counter = 0; blink = !blink; }
		++counter;
	}
	stdin_pop(); //Remove trailing newline
	vmove(location+MEMORY.INDEX.stdin);
	print(" "); //erase leftover blinky thing
	return MEMORY.STDIO.stdout;
}

void stdin_clear(void) {
	MEMORY.FLAGS.stdout = true;
	MEMORY.STDIO.stdout[0] = '\0';
	MEMORY.INDEX.stdin=0;
}

void stdin_push(char c) {
	MEMORY.FLAGS.stdout = true;
	if(MEMORY.INDEX.stdin < STD_MAX) {
		MEMORY.STDIO.stdout[MEMORY.INDEX.stdin] = c;
		MEMORY.STDIO.stdout[MEMORY.INDEX.stdin+1] = '\0';
		MEMORY.INDEX.stdin++;
	} 
	else {
		stdin_push(STD_MAX-1);
	}
}

char stdin_pop(void) {
	MEMORY.FLAGS.stdout = true;
	if(MEMORY.INDEX.stdin > 0) {
		MEMORY.INDEX.stdin--;
		MEMORY.STDIO.stdout[MEMORY.INDEX.stdin] = '\0';
		return MEMORY.STDIO.stdout[MEMORY.INDEX.stdin-1];
	} else {
		stdin_clear();
		return '\0';
	}
}

char stdin_peek(void) {
	if(MEMORY.INDEX.stdin > 0) {
		return MEMORY.STDIO.stdout[MEMORY.INDEX.stdin-1];
	} else {
		return '\0';
	}
}
