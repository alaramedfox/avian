/*
 *	Popcorn Kernel
 *	  File:		command.h
 *	  Purpose:	Main Popcorn command processor
 */
 
/* Core commands
		l 	List items in directory
		x	Execute program
		t	transfer file from a -> b
		c	copy file or folder
		m	make a file, folder, etc
		!	throw error
		?	request value
		=	set value
		<	get data from stream
		>	push data to stream
		q	list system data
*/



#include "lxtcm/list.h"			
#include "lxtcm/exec.h"
#include "lxtcm/transfer.h"
#include "lxtcm/help.h"


/* Declare functions */
void process_raw_input(const char*);		//Raw string from keyboard


void process_raw_input(const char input[])
{
	char *first_word = strword(input,0);
	if(strcomp(first_word,"clear")) 		{ clear(); }
	else if(strcomp(first_word,"l")) 	{ lxtcm_list(input); }
	else if(strcomp(first_word,"help")) { lxtcm_help_basic(); }
	else {
		lxtcm_help_unknown(first_word);
	}
}


