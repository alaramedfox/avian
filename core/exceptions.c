#define EXCEPTIONS_C_SOURCE
#include <exceptions.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/exceptions.c
 *		Purpose:	Exception handling
 */
#include <vga.h>
#include <stdio.h>
#include <color.h>

void throw(char* message)
{
	vga_setcolor(C_CRITICAL);
	print("\n");
	print(message);
	while(true);
}