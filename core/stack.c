#define STACK_C_SOURCE
#include <stack.h>
/*
 *		Avian Project
 *			File:		/lib/stack.c
 *			Purpose:	Create stack of objects
 */

char* stack_str(stack* s)
{
	return s->data;
}

void push_str(stack* s, char str[])
{
	for(size_t i=0; str[i] != '\0'; i++) {
		push(s,str[i]);
	}
}

stack* new_stack(const size_t size)
{
	stack* stk = (stack*) malloc(size*sizeof(stack));
	stk->data = (char*)malloc(size);
	stk->size = 0;
	stk->max = size;
	return stk;
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
		s->data[s->size] = '\0';
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