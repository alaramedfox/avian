#define STDPRINT_C_SOURCE
#include <stdio.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/lib/stdprint.c
 *		Purpose:	Functions that print to the screen
 */
 
void addch(const char c)
{
	vga_write(c);
	vga_increment();
}
 
void print(const char str[])
{
	for(size_t i=0; str[i] != '\0'; i++) {
		switch(str[i])
		{
			case '\n': vga_newline(); break;
			case '\r': vga_creturn(); break;
			case '\t': vga_tabchar(); break;
			
			default: addch(str[i]); break;
		}
	}
}