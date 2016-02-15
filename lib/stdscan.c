#define STDSCAN_C_SOURCE
#include <stdio.h>
#include <envar.h>
#include <stack.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/lib/stdscan.c
 *		Purpose:	Source for standard input functions
 */
 
/* Stdin housed in keyboard.c */
extern stack* stdin;

char* scan(void)
{
	ENVAR.FLAGS.listen = true;
	delete_stack(stdin);
	stdin = new_stack(128);
	uint16_t location = vga_getloc();
	
	while(peek(stdin) != '\n')
	{
		if(ENVAR.FLAGS.keypress)
		{
			vga_moveptr(location);
			print(stack_str(stdin));
			move_cursor(vga_getrow(), vga_getcol());
			addch(' ');
			ENVAR.FLAGS.keypress = false;
		}
	}
	
	while(peek(stdin) == '\n') {
		pop(stdin); //remove trailing newline
	}
	
	ENVAR.FLAGS.listen = false;
	char* input = stack_str(stdin);
	return input;
}