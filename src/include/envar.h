#ifndef ENVAR_H_INCLUDED
#define ENVAR_H_INCLUDED
#include <types.h>
/*
 *		Popcorn Kernel - Bryan Webb
 *		File:		/include/envar.h
 *		Purpose:	Header for envar.c
 */

enum __ECHOSTATE;
struct __GLOBAL;
struct __FLAGS;
struct __ENVAR;
extern struct __ENVAR ENVAR;
void ENVAR_init(void);
 
#endif