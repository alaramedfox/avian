#ifndef ENVAR_H_INCLUDED
#define ENVAR_H_INCLUDED
#include <envar.h>
#include <types.h>
/*
 *		Avian Project - Bryan Webb
 *		File:		/include/envar.h
 *		Purpose:	Header for envar.c
 */

enum __ECHOSTATE
{
	NOECHO, ECHO, PASSWD,
};

struct __GLOBAL {
	byte	   echostate;					//Whether or not to echo STDIN
	byte	   tabsize;						//Size of the tabs
};

struct __FLAGS {
	bool 		listen;						//If set, signal to catch input
	bool 		repaint;						//If set, signal need to repaint
	bool		captain;						//If set, allow unrestricted access
	bool		shift;						//If set, flag for the uppercase charset
	bool		caps;							//If set, permanant flag for uppercase
	bool		keypress;					//True when key is pressed
	bool		meminfo;
	bool		general[16];				//General use flags
};

struct __ENVAR {
	struct __FLAGS		FLAGS;
	struct __GLOBAL	GLOBAL;
	
} ENVAR;

void ENVAR_init(void);
 
#endif