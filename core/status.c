#define STATUS_C_SOURCE
#include <types.h>
/*
 *		Avian Project - Bryan Webb
 *		File:			/core/status.c
 *		Purpose:		Status structure, for return value of functions	
 */
 

 
inline static status_t STATUS(int8_t l,char* m)
{
	status_t stat;
	stat.level = l;
	stat.message = m;
	return stat;
}

inline status_t OKAY(void) 	{ return STATUS(S_OKAY,""); }
inline status_t FAIL(char* e) { return STATUS(S_FAIL,e);  }
inline status_t WARN(char* e) { return STATUS(S_WARN,e);  }