#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		util.h
 *			Purpose:	Provide essential core functions
 */
 
/* Calculate the size of a null-terminated string literal */
#define C_STR_MAX_SIZE	512
size_t strlen(const char *c_str)
{
	size_t size=0;
	while(c_str[size] != '\0' && size < C_STR_MAX_SIZE) {
		++size;
	}
	return size;
}

#endif