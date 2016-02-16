#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
/*
 *		Avian Project
 *			File:		util.h
 *			Purpose:	Provide essential core functions
 */
 
#include <types.h>

size_t strlen(const char*);

char* itoa(int,int);
char* itoa_bytes(int32_t);
 
#endif