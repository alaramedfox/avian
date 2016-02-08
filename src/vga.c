/*
 *		Popcorn Kernel
 *			File:		vga.h
 *			Purpose:	Video output handler
 */
 
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
 
#define THIS_COL	MEMORY.INDEX.vram%(VGA_C)			//Current column in VRAM
#define THIS_ROW	MEMORY.INDEX.vram/(VGA_C)			//Current row in VRAM
 
class __STDOUT
{
	private:
	
	byte 	*vram;
	size_t vptr;
	color_t globalcolor;
	
	public:
	
	/* Defines */
	const size_t BYTES_SIZE = 3840;	//Total number of VRAM bytes
	const size_t BYTES_COLS = 160;	//Define the number of column bytes
	const size_t BYTES_ROWS = 48;		//Define the number of row bytes
	
	const size_t CHARS_SIZE = 1920;  //Total number of screen characters
	const size_t CHARS_COLS = 80;		//Width of screen in character cells
	const size_t CHARS_ROWS = 24;		//Height of screen in character cells
	
	/* Constructors */
	__STDOUT();
	
	/* Setters and getters */
	size_t getcol	(void);				//Safely return the current column of the cursor
	size_t getrow	(void);				//Safely return the current row of cursor
	size_t getloc 	(void);				//Safely return the 1D vptr location
	
	/* Screen manipulation */
	void scroll	(void);				//Scrolling screen support
	void clear	(void);				//Clear the screen
	void move	(byte,byte);		//Move cursor with 2D
	void move	(int16_t);			//Move cursor with 1D
	void newline(void);				//Move to next line, starting in first column
	void creturn(void);				//Move to next line, staying in same column
	
	/* Printing */
	void write	(char);				//Safely write to vram[vptr]
	void color	(color_t);			//Safely write to vram[vptr+1]
	void print	(string);			//Prints a string starting at cursor
	void printf	(color_t,string);	//Print with basic formatting

};

__STDOUT::__STDOUT()
{
	vram = (byte*)0xb8000;
	vptr = 0;
	globalcolor = 0x07;
}

size_t __STDOUT::getcol()
{
	return THIS_COL;
}

size_t __STDOUT::getrow()
{
	return THIS_ROW;
}

size_t __STDOUT::getloc()
{
	return vptr;
}

void __STDOUT::newline(void) 
{
	move(THIS_ROW+1,0);
}

void __STDOUT::creturn(void)
{
	move(THIS_ROW+1,THIS_COL);
}

void __STDOUT::setcolor_pair(color_t fg, color_t bg) 
{
	globalcolor = palette(fg,bg);
}

void __STDOUT::setcolor(color_t c) 
{
	globalcolor = c;
}

void __STDOUT::move(byte row, byte col) 
{
	if(row > VGA_R || col > VGA_C) return;

	vptr = row * VGA_C + col;
	if(vptr > VRAM_CHARS-1) { scroll(); }
}

void __STDOUT::move(int16_t v) 
{
	if(v >= VRAM_CHARS) {
		scroll(); 
		move(v-VGA_C); 
	}
	else {
		vptr = v; 
	}
}

void __STDOUT::clear()
{
	vptr = 0;
	setcolor(C_TERMINAL);
	for(count_t i=0; i<VRAM_CHARS; i++) {
		write('.');
		vptr++;
	}
	vptr = 0;
}

/* Safely write a single character to vram without segfault */
void __STDOUT::write(char value) 
{
	if(vptr > VRAM_CHARS-1) { 
		scroll(); 
		write(value); 
	}
	else { 
		vram[vptr*2] = value; 
	}
}

void __STDOUT::color(color_t value) 
{
	if(vptr > VRAM_CHARS) { 
		scroll(); 
		color(value); 
	}
	else { 
		vram[vptr*2+1] = value; 
	}
}

void __STDOUT::scroll(void) 
{
	/* Shift every row up one row */
	for(count_t i=0; i<=VRAM_LENGTH-VRAM_C; i++) {
		vram[i] = vram[i+VRAM_C];
	}

	move(23,0);
	for(count_t i=0;i<80; i++) {
		write(' ');
		color(globalcolor);
		vptr++;
	}
	/* Set cursor to start of line */
	move(23,0);
}

void __STDOUT::print(string str) 
{
	/* Write the char* to VRAM starting at MEMORY.INDEX.vram */
	for(count_t i=0; i<str.size(); i++) {
	
		/* Ignore (trailing) newline if kernel is listening to the keyboard */
		switch(str[i])
		{
			case '\n': newline(); break;
			case '\0': break;
			case '\b': break;
			case '\r': creturn(); break;
			default: write(str[i]);
						color(globalcolor);
						vptr++;
						break;
		}
	}
}

void __STDOUT::printf(color_t color, string str) 
{
	color_t old_color = globalcolor;				//Remember previous color
	setcolor(color);									//Change to new color
	print(str);											//Call print()
	setcolor(old_color);								//Revert to the old color
}