#define STATUS_C_SOURCE
#include <types.h>
/*
 *		Avian Project - Bryan Webb
 *		File:			/core/status.c
 *		Purpose:		Status structure, for return value of functions	
 */
 

 
inline status_t STATUS(int8_t l, char* m)
{
	status_t stat;
	stat.level = l;
	stat.message = m;
	return stat;
}

inline status_t STATUS_OK(void)
{
	return STATUS(OK,"OK");
}