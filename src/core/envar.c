#define ENVAR_C_SOURCE
#include <envar.h>
/*
 *		Popcorn Kernel - Bryan Webb
 *		File:		/core/envar.c
 *		Purpose:	Contains and handles enviornmental variables
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
	bool		general[16];				//General use flags
};

struct __ENVAR {
	struct __FLAGS		FLAGS;
	struct __GLOBAL	GLOBAL;
	
} ENVAR;

void ENVAR_init(void)
{
	ENVAR.FLAGS.listen 	= false;
	ENVAR.FLAGS.repaint 	= false;
	ENVAR.FLAGS.keypress = false;
	ENVAR.GLOBAL.echostate = ECHO;
	ENVAR.FLAGS.captain = false;
	ENVAR.GLOBAL.tabsize = 4;
}