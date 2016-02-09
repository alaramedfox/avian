#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		util.h
 *			Purpose:	Provide essential core functions
 */
 
 
/* Calculate the size of a null-terminated string literal */
#define C_STR_MAX_SIZE	512
inline size_t strlen(const char* str)
{
	size_t size=0;
	while(str[size] != '\0' && size < C_STR_MAX_SIZE) {
		++size;
	}
	return size;
}

#endif