#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED
/* ======================================================================== */
/*    Avian Kernel   Bryan Webb (C) 2016
/*    File:          /core/include/defs.h
/*    Purpose:       Global function macro and misc definitions
/* ======================================================================== */

/* Bit manipulation macros */
#define bitmask(var,bit)	var & ~(1 << bit)
#define bitset(var,bit)		var | (1 << bit)
#define bitcheck(var,bit)	var & (1 << bit)

/* This creates a `foreach' style loop */
#define foreach(var, count)	for(int var=0; var<count; var++)
		
/* This compresses the packed attribute into something smaller */
#define FLAT   __attribute__((packed))

#endif