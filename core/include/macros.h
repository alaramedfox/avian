#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED
#pragma once
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/macros.h
 *		Purpose:	Define a number of global macro utilities
 */
 
/* __attribute__ ((x)) aliases */
#define hot 		__attribute__((hot))
#define packed		__attribute__((packed))

/* Custom control flow operators */
#define until(exp) 	while(!(exp)) 	//Perl-like until loop
#define unless(exp)	if(!(exp))   	//Perl-like unless switch
#define STR(x)			itoa(x,10)
#define QW(x)			#x
 
#endif