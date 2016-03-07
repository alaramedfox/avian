#define STACK_C_SOURCE
// ======================================================================= */
//		Avian Kernel   Bryan Webb (C) 2016
//		File:		      avian/stack.c
//		Purpose:	      Create stack of objects
// ======================================================================= */
 
#include <stack.h>

char* stack_str(stack* s)
{
	return (char*)s->data;
}

void push_str(stack* s, char str[])
{
	for(size_t i=0; str[i] != '\0'; i++) {
		push(s,str[i]);
	}
}

stack* new_stack(const size_t size)
{
	stack* stk = (stack*) malloc(sizeof(stack));
	stk->data = (byte*) malloc(size);
	stk->size = 0;
	stk->max = size;
	return stk;
}

void delete_stack(stack *s)
{	
	free(s->data);
	free(s);
}

void push(stack* s, byte c)
{
	if(s->size < s->max) {
		s->data[s->size] = c;
		s->size++;
		s->data[s->size] = '\0';
	}
}

byte pop(stack *s)
{
	if(!empty(s)) {
		char c = peek(s);
		s->size--;
		s->data[s->size] = '\0';
		return c;
	} else return '\0';
}

byte peek(stack *s)
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