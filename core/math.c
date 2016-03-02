#define MATH_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/math.c
 *		Purpose:	Implementation of the Standard Library mathematical functions
 */
 
#include <stdlib.h>
#include <util.h>
#include <vga.h>

div_t div(int num, int den)
{
	if(den == 0) { print("Error!\n"); while(true); }
	div_t result;
	result.quot = num / den;
	result.rem = num % den;
	return result;
}

ldiv_t ldiv(int num, int den)
{
	if(den == 0) { print("Error!\n"); while(true); }
	ldiv_t result;
	result.quot = num / den;
	result.rem = num % den;
	return result;
}