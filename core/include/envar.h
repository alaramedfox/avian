#ifndef ENVAR_H_INCLUDED
#define ENVAR_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/envar.h
 *		Purpose:	Global variables - communication between
 *					many different parts of the kernel
 */
 
#include <types.h>

enum __ECHOSTATE
{
	NOECHO, ECHO, PASSWD,
};

struct __GLOBAL {
	byte	   echostate;		//Whether or not to echo STDIN
	byte	   tabsize;			//Size of the tabs
	status_t status;			//Error capturing
};

struct __FLAGS {
	bool 		listen;			//If set, signal to catch input
	bool 		repaint;			//If set, signal need to repaint
	bool		captain;			//If set, allow unrestricted access
	bool		shift;			//If set, flag for the uppercase charset
	bool		caps;				//If set, permanant flag for uppercase
	bool		keypress;		//True when key is pressed
	
	bool		error;			//True when a component flags an error
};

struct __ENVAR {
	struct __FLAGS		FLAGS;
	struct __GLOBAL	GLOBAL;
	
} ENVAR;

void ENVAR_init(void);
 
#endif