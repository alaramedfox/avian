#define STACK_C_SOURCE
#include <stack.h>
/*
 *		Popcorn Kernel
 *			File:		/lib/stack.c
 *			Purpose:	Create stack of objects
 */



stack* new_stack(const size_t size)
{
	return (stack*) malloc(size + 2*sizeof(size_t));
}

void delete_stack(stack *s)
{	
	free(s);
}

void push(stack* s, char c)
{
	if(s->size < s->max) {
		s->data[s->size] = c;
		s->size++;
	}
}

char pop(stack *s)
{
	if(!empty(s)) {
		char c = peek(s);
		s->size--;
		s->data[s->size] = '\0';
		return c;
	} else return '\0';
}

char peek(stack *s)
{
	if(!empty(s)) {
		return s->data[s->size-1];
	} else return '\0';
}

bool empty(stack *s)
{
	return s->size == 0;
}

bool full(stack *s)
{
	return s->size == s->max;
}