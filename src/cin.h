#ifndef CIN_H_INCLUDED
#define CIN_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		stdin.c
 *			Purpose:	Abstracts input handler
 */

#include "string.h"
 
class CIN
{
	private:
	string buffer;

	public:
	void clear	(void);				//Flush the input buffer
	void push	(char);				//Push character to stdin
	char pop		(void);				//Remove and return last char in stdin
	char peek	(void);				//Return but do not remove last char
	string scan	(void);				//Pause execution and return a line of input
	char getch  (void);				//Pause execution and return a single character
	
	string getbuffer(void) { return buffer; }
} stdin;


string CIN::scan() 
{
	/*
	MEMORY.FLAGS.listen = true;			//Set flag to listen for input
	buffer = string();						//Make sure stdin is empty before reading
	int16_t location = stdout.getloc();	//Remember starting vptr location
	move_cursor(stdout.getrow(), stdout.getcol());
	while(true) {
		
		if(buffer.peek() == '\n') {
			buffer.pop();
			MEMORY.FLAGS.repaint = false;
			MEMORY.FLAGS.listen = false;
			return buffer;
			break;
		}
		if(MEMORY.FLAGS.repaint) {
			MEMORY.FLAGS.repaint = false;
			stdout.move(location);
			stdout.print_raw(buffer);
			stdout.write(' ');
			move_cursor(stdout.getrow(),stdout.getcol());
		}
			
	
	} */ /* Loop ends when user presses return */
}

void CIN::clear(void) 
{
	buffer = string();	//Reset to empty string
}

void CIN::push(char c)
{
	MEMORY.FLAGS.repaint = true;			//Set repaint flag
	//buffer.push(c);
}

char CIN::pop(void) 
{
	MEMORY.FLAGS.repaint = true;	//Flag for repaint
	//return buffer.pop();
}

char CIN::peek(void) 
{
	//return buffer.peek();
}

#endif
