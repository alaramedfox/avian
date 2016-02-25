#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		core/include/stdlib.h
 *		Purpose:	Header for the C Standard Library
 */
 
#include <stdint.h>
#include <stddef.h>

/* Prototypes for string-to-int functions */
double 	atof(const char *str);
int 		atoi(const char *str);
int 		atol(const char *str);

double	strtod(const char *str, char **endptr);
int		strtol(const char *str, char **endptr, int base);

/*		
 *		MEMORY - Defined in mmap.c
 */
void*		calloc(size_t nitems, size_t size);
void		free(void *ptr);
void*		malloc(size_t size) __attribute__ ((malloc));
void*		realloc(void *ptr, size_t size);

typedef struct __DIV
{
	int quot;
	int rem;
	
} div_t;

typedef struct __LDIV
{
	int quot;
	int rem;

} ldiv_t;

/*
 *		MATH - Defined in math.c
 */
div_t 	div(int num, int den);
ldiv_t 	ldiv(int num, int denom);

#endif