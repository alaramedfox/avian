/*
 *		Popcorn Kernel (C) Bryan Webb
 *			File:		escapes.h
 *			Purpose:	Contains a hash of escapes and their respective characters
 */
 
extern const char HLINE1, HLINE2, HLINE3,
						VLINE1, VLINE2,
						BLOCK1, BLOCK2, BLOCK3, BLOCK4;
 
char escape(string code)
{
	     if(code == "h1") return(HLINE1);
	else if(code == "h2") return(HLINE2);
	else if(code == "h3") return(HLINE3);
	else if(code == "v1") return(VLINE1);
	else if(code == "v2") return(VLINE2);
	else if(code == "b1") return(BLOCK1);
	else if(code == "b2") return(BLOCK2);
	else if(code == "b3") return(BLOCK3);
	else if(code == "b4") return(BLOCK4);
	else return '?';
}