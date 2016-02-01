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

void lxtcm_unknown()
{
	print("\nLXTCM: Unreconized command");
}
			
void lxtcm_list()
{
	print("\nLXTCM List function");
}

void lxtcm_exec()
{
	print("\nLXTCM Execute function");
}

void lxtcm_transfer()
{
	print("\nLXTCM Transfer function");
}			

/* Declare functions */
void process_raw_input(const char*);		//Raw string from keyboard


void process_raw_input(const char input[])
{
	char *first_word = strword(input,0);
	print(" (");
	print(first_word);
	print(")");
	if(strcomp(first_word,"clear")) {
		clear();
	}
	else if(strcomp(first_word,"list")) {
		lxtcm_list();
	}
	else {
		lxtcm_unknown();
	}
}


