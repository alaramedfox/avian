#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/lib/include/stdio.h
 *		Purpose:	Header for standard input and output functions
 */
#include <types.h>
#include <keyboard.h>
#include <mmap.h>
#include <vga.h>

/* Output functions, defined in /lib/stdprint.c */
void addch(const char);							//Print a single character
void print(const char[]);						//Standard print function
void printf(const char[], const char[]);	//Print with special formatting
void println(const char[]);					//Append newline to output

void print_xy(byte, byte, const char[]);	//Print to XY coordinate
void print_ptr(word, const char[]);			//Print to vptr coordinate
void print_raw(const char[]);					//Do not format special characters

/* Input functions, defined in /lib/stdscan.c */
char* scan(void);									//Wait and capture input until return
char getch(void);									//Wait until a single key is pressed




#endif