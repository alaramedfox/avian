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
 
#define VRAM_C	80*2							//Define the number of char columns
#define VRAM_R	24*2							//Define the number of char rows
#define VGA_C	80								//Width of VGA in bytes
#define VGA_R	24								//Height of VGA in bytes

#define VRAM_START	0xb8000				//Actual address of system VRAM
#define VRAM_LENGTH	80*24*2				//Length of VRAM in bytes
#define VRAM_CHARS	VRAM_LENGTH/2		//Length of VRAM in words

#define CHAR_NEWLINE 0xA					//Define the newline char '\n'
#define CHAR_TAB	 0x9						//Define the tab char '\t'

#define THIS_COL	vptr%(VGA_C)			//Current column in VRAM
#define THIS_ROW	vptr/(VGA_C)			//Current row in VRAM


extern const char HLINE1, HLINE2, HLINE3;
extern const char VLINE1, VLINE2;
extern const char BLOCK1, BLOCK2, BLOCK3, BLOCK4;

extern void i_listen();						//Declare the listener in the ASM file
extern void i_ignore();						//Declare the un-listener in ASM

char 	*vram = (char*)VRAM_START;			//Access to system video RAM
int16_t 	vptr = 0;							//pointer to current vram spot
color_t 	this_color = 0x07;				//Register for current color
bool	 	STDIN_FLAG=false;					//0 = no update, 1 = update
char 		std_stream[256];					//The input stream buffer
size_t	stdin_size=0;						//Size of buffer
bool 		VUPDATE=false;						//Flag to update something

void vmove(int16_t);							//Safely move the vram pointer.
void print(const char str[]);				//Prints a string starting at vpt
void clear(void);								//Clears the screen
void move(int,int);							//Moves to 2D location in VRAM
void newline(void);							//Increment to start of next row
void setcolor_pair(color_t,color_t);	//Change the screen color: pairs
void setcolor(color_t);						//Change the screen color: palette
void scroll(void);							//Scrolling screen support
void vram_char(char);						//Safely write to vram[vptr]
void vram_color(color_t);					//Safely write to vram[vptr+1]
	
void stdin_clear();							//Flush the input buffer
void stdin_push(char);						//Push character to stdin
char stdin_pop();								//Remove and return last char in stdin
char stdin_peek();							//Return but do not remove last char

void clear(void) {
	for(int i=0; i<VRAM_CHARS; i++) {
		vptr = i;
		vram_char(' ');
		vram_color(palette(C_WHITE,C_BLACK));
	}
	vptr = 0;
}

inline void newline(void) {
	move(THIS_ROW+1,0);
}

inline void move(int row, int col) {
	vptr = (col + VGA_C * row)-1;
	if(vptr >= VRAM_CHARS) { scroll(); }
}

inline void vmove(int16_t v) {
	if(v >= VRAM_CHARS) { scroll(); vmove(v-VGA_C); }
	else { vptr = v; }
}

inline void setcolor_pair(color_t fg, color_t bg) {
	this_color = palette(fg,bg);
}
inline void setcolor(color_t color) {
	this_color = color;
}

/* Safely write to vram without segfault */
void vram_char(char value) {
	if(vptr >= VRAM_CHARS) { scroll(); vram_char(value); }
	else { vram[vptr*2] = value; }
}

void vram_color(color_t value) {
	if(vptr >= VRAM_CHARS) { scroll(); vram_color(value); }
	else { vram[vptr*2+1] = value; }
}

void scroll(void) {
	/* Shift every row up one row */
	for(int i=0; i<=VRAM_LENGTH-VRAM_C; i++) {
		vram[i] = vram[i+VRAM_C];
	}
	
	move(23,0);
	for(int i=0;i<80; i++) {
		vptr++;
		vram_char(' ');
		vram_color(this_color);
	}
	/* Set cursor to start of line */
	move(23,1);
}

void print(const char str[]) {
	/* Write the string to VRAM starting at vptr */
	for(int i=0; str[i] != '\0'; i++) {
		vptr++;
		
		if(str[i] == '&') {
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
		vram_color(this_color);
	}
	return;
}

void printf(color_t color, const char str[]) {
	color_t old_color = this_color;
	setcolor(color);
	print(str);
	setcolor(old_color);
}

char* stdin(int8_t FLAG) {
	VUPDATE = true;	//Flag for repaint initially
	stdin_clear();
	int16_t location = vptr;
	int32_t counter = 0;
	bool blink = true;
	while(stdin_peek() != '\n') {
		vmove(location);
		
		if(VUPDATE)	{ //Only update screen if a key was pressed
			switch(FLAG) {
				case ECHO: 		print(std_stream); 						break;
				case PASSWD: 	for(size_t i=0;i<stdin_size;i++)
					    			{ print("*"); } 							break;
				case NOECHO:													break;
				default: print(std_stream); 								break;
			}
			VUPDATE = false; //De-flag the update flag
		}
		vmove(location+stdin_size);
		blink?print("_ "):print("  ");
		if(counter > 65534*3) { counter = 0; blink = !blink; }
		++counter;
	}
	stdin_pop(); //Remove trailing newline
	vmove(location+stdin_size);
	print(" "); //erase leftover blinky thing
	return std_stream;
}

void stdin_clear(void) {
	VUPDATE = true;
	std_stream[0] = '\0';
	stdin_size=0;
}

void stdin_push(char c) {
	VUPDATE = true;
	if(stdin_size < STD_MAX) {
		std_stream[stdin_size] = c;
		std_stream[stdin_size+1] = '\0';
		stdin_size++;
	} 
	else {
		stdin_push(STD_MAX-1);
	}
}

char stdin_pop(void) {
	VUPDATE = true;
	if(stdin_size > 0) {
		stdin_size--;
		std_stream[stdin_size] = '\0';
		return std_stream[stdin_size-1];
	} else {
		stdin_clear();
		return '\0';
	}
}

char stdin_peek(void) {
	if(stdin_size > 0) {
		return std_stream[stdin_size-1];
	} else {
		return '\0';
	}
}
