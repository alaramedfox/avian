#ifndef ENVAR_H_INCLUDED
#define ENVAR_H_INCLUDED
// ======================================================================== */
//		Avian Kernel   Bryan Webb (C) 2016
//		File:		      avian/include/envar.h
//		Purpose:	      Global variables - communication between
//					      many different parts of the kernel
// ======================================================================== */
 
#include <stdlib.h>

enum __ECHOSTATE
{
	NOECHO, ECHO, PASSWD,
};

struct __GLOBAL {
	byte	   echostate;		//Whether or not to echo STDIN
	byte	   tabsize;			//Size of the tabs
	
} FLAT;

struct __FLAGS {
	bool 		listen: 1;			//If set, signal to catch input
	bool 		repaint: 1;			//If set, signal need to repaint
	bool		captain: 1;			//If set, allow unrestricted access
	bool		shift: 1;			//If set, flag for the uppercase charset
	bool		caps: 1;				//If set, permanant flag for uppercase
	bool		keypress: 1;		//True when key is pressed
	bool		mmupdate: 1;
	
	bool		error: 1;			//True when a component flags an error
	
} FLAT;

struct __ENVAR {
	struct __FLAGS		FLAGS;
	struct __GLOBAL	GLOBAL;
	
} ENVAR;

void ENVAR_init(void);
 
#endif