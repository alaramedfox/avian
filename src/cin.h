#ifndef CIN_H_INCLUDED
#define CIN_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		stdin.c
 *			Purpose:	Abstracts input handler
 */

#include "string.h"
 
class CIN {
private:
	stringstream buffer;

public:
	CIN();
	
	void clear	(void);				//Flush the input buffer
	void push	(char);				//Push character to stdin
	char pop		(void);				//Remove and return last char in stdin
	char peek	(void);				//Return but do not remove last char
	string scan	(void);				//Pause execution and return a line of input
	char getch  (void);				//Pause execution and return a single character
	
} stdin;

CIN::CIN()
{
	buffer = stringstream(128);
}

string CIN::scan() 
{
	ENVAR.FLAGS.listen = true;			//Set flag to listen for input
	int16_t location = stdout.getloc();	//Remember starting vptr location
	move_cursor(stdout.getrow(), stdout.getcol());
	buffer.clear();
	
	while(buffer.peek() != '\n') {
		//if(buffer.peek() == '\n') break;
	
		if(ENVAR.FLAGS.repaint) 
		{
			ENVAR.FLAGS.repaint = false;
			stdout.move(location);
			stdout.print(buffer.str());
			stdout.write(' ');
			move_cursor(stdout.getrow(),stdout.getcol());
		}
			
	
	} /* Loop ends when user presses return */
	buffer.pop(); //Trim trailing newline
	
	return buffer.str();
}

void CIN::clear(void) 
{
	buffer.clear();	//Reset to empty string
}

void CIN::push(char c)
{
	ENVAR.FLAGS.repaint = true;			//Set repaint flag
	buffer.push(c);
}

char CIN::pop(void) 
{
	ENVAR.FLAGS.repaint = true;	//Flag for repaint
	return buffer.pop();
}

char CIN::peek(void) 
{
	return buffer.peek();
}

#endif
