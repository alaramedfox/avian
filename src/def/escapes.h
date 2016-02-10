#ifndef ESCAPES_H_INCLUDED
#define ESCAPES_H_INCLUDED
/*
 *		Popcorn Kernel (C) Bryan Webb
 *			File:		escapes.h
 *			Purpose:	Contains a hash of escapes and their respective characters
 */
 
extern const char HLINE1, HLINE2, HLINE3,
						VLINE1, VLINE2,
						BLOCK1, BLOCK2, BLOCK3, BLOCK4;
 
char escape(char code)
{
	     if(code == 'h') return(HLINE1);
	else if(code == 'H') return(HLINE2);
	else if(code == 'v') return(VLINE1);
	else if(code == 'V') return(VLINE2);
	else if(code == 'b') return(BLOCK1);
	else if(code == 'B') return(BLOCK2);
	else if(code == 's') return(BLOCK3);
	else if(code == 'S') return(BLOCK4);
	else return '?';
}
#endif