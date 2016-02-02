/*
 *	Popcorn Kernel
 *	  File:		lxtem.h
 *	  Purpose:	Main Popcorn command processor
 */
 
/* Core commands
		l 	List items in directory
		x	Execute program
		t	transfer file from a -> b
		e	throw error
		m	make a file, folder, etc
		?	request value
		=	set value
		<	get data from stream
		>	push data to stream
		q	list system data
*/



#include "lxtem/list.c"			
#include "lxtem/exec.c"
#include "lxtem/transfer.c"
#include "lxtem/help.c"
#include "lxtem/edit.c"


/* Declare functions */
void process_raw_input(const string);		//Raw string from keyboard


void process_raw_input(const string input)
{
	string first_word = strword(input,0);
	if(strcomp(first_word,"clear")) 		{ clear(); }
	else if(strcomp(first_word,"l")) 	{ lxtem_list(input); }
	else if(strcomp(first_word,"t"))		{ lxtem_transfer(input); }
	else if(strcomp(first_word,"x"))		{ lxtem_exec(input); }
	else if(strcomp(first_word,"e"))		{ lxtem_edit(input); }
	else if(strcomp(first_word,"help")) { lxtem_help_basic(); }
	else {
		lxtem_help_unknown(first_word);
	}
}


