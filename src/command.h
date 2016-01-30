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
			
void lxtcm_list()
{
	print("\nLXTCM List function");
}

void lxtcm_exec()
{
	print("LXTCM Execute function");
}

void lxtcm_transfer()
{
	print("LXTCM Transfer function");
}			

/* Declare functions */
void process_raw_input(const char*);		//Raw string from keyboard


void process_raw_input(const char input[])
{
	if(string_compare(input,"clear"))
	{
		clear();
	}
}


