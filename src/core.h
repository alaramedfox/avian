#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		core.h
 *			Purpose:	Include essential system sources
 */

#ifndef DEFINES_H_INCLUDED
#error "System core requires the defines.h header."
#endif

#include "sys/icxxabi.c"
#include "sys/memory.c"
#include "sys/idt.c"
#include "sys/envar.c"

#else
#warning core.h should only be included once
#endif