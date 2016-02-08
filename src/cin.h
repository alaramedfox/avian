/*
 *		Popcorn Kernel
 *			File:		stdin.c
 *			Purpose:	Abstracts input handler
 */

#include "string.h"
 
class __STDIN
{
	private:
	string buffer;

	public:
	void clear(void);					//Flush the input buffer
	void push(char);					//Push character to stdin
	char pop(void);					//Remove and return last char in stdin
	char peek(void);					//Return but do not remove last char
	string scan(void);
}


string __STDIN::scan() 
{
	MEMORY.FLAGS.listen = true;			//Set flag to listen for input
	MEMORY.FLAGS.repaint = true;			//Flag for repaint initially
	clear();										//Make sure stdin is empty before reading
	int16_t location = stdout.getloc();	//Remember starting vptr location
	string buffer;
	while(buffer.peek() != '\n') {
		
		if(MEMORY.FLAGS.repaint) {
			stdout.move(location);
			stdout.print(buffer);
			MEMORY.FLAGS.repaint = false;
			move_cursor(THIS_ROW,THIS_COL);
		}
		
	
	} /* Loop ends when user presses return */
	MEMORY.FLAGS.listen = false;					//De-flag input polling
	return buffer;
}

void __STDIN::clear(void) 
{
	MEMORY.FLAGS.repaint = true;
	buffer.clear();	//Reset to empty string
}

void __STDIN::push(char c)
{
	MEMORY.FLAGS.repaint = true;			//Set repaint flag
	buffer.push(c);
}

char __STDIN::pop(void) 
{
	MEMORY.FLAGS.repaint = true;	//Flag for repaint
	return buffer.pop();
}

char __STDIN::peek(void) 
{
	return buffer.peek();
}
