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

namespace iostream 
{
	/* Standard output stream*/
	#include "stdout.h"
	__STDOUT stdout;
	
	/* Standard input stream */
	#include "stdin.h"
	__STDIN stdin;
	

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
		MEMORY.FLAGS.listen = true;			//Set flag to listen for input
		MEMORY.FLAGS.repaint = true;			//Flag for repaint initially
		clear();										//Make sure stdin is empty before reading
		int16_t location=MEMORY.INDEX.vram;	//Remember starting vptr location
		string buffer;
		while(buffer.peek() != '\n') {
			
			if(MEMORY.FLAGS.keypress) {
				buffer.push(MEMORY.IO.cin);
				MEMORY.IO.cin = '\0';
				MEMORY.FLAGS.keypress = false;
				MEMORY.FLAGS.repaint = true;
			}
			
			if(MEMORY.FLAGS.repaint) {
				stdout::move(location);
				stdout::print(buffer);
				MEMORY.FLAGS.repaint = false;
				move_cursor(THIS_ROW,THIS_COL);
			}
			
		
		} /* Loop ends when user presses return */
		MEMORY.FLAGS.listen = false;					//De-flag input polling
		return buffer;
	}

	void clear(void) 
	{
		MEMORY.IO.instream.clear();	//Reset to empty string
		MEMORY.IO.cin = '\0';
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
		return MEMORY.IO.instream.pop();
	}

	char peek(void) 
	{
		return MEMORY.IO.instream.peek();
	}

} // namespace stdin
