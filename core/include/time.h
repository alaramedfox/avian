#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/time.h
 *		Purpose:	Handle the PIT and time-related functions
 */
#include <types.h>

void	time_init(word);

uint32_t clock(void);
void	sleep(uint32_t);	//Sleep for N ms
void	pit_handler(void);

#endif