#define ENVAR_C_SOURCE
#include <envar.h>
/*
 *		Avian Project - Bryan Webb
 *		File:		/core/envar.c
 *		Purpose:	Define enviornmental global variables
 */


void ENVAR_init(void)
{
	ENVAR.FLAGS.listen 	= false;
	ENVAR.FLAGS.repaint 	= false;
	ENVAR.FLAGS.keypress = false;
	ENVAR.GLOBAL.echostate = ECHO;
	ENVAR.FLAGS.captain = false;
	ENVAR.GLOBAL.tabsize = 4;
	ENVAR.GLOBAL.status = OKAY();
}