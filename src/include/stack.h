#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED
// ======================================================================== */
//		Avian Kernel   Bryan Webb (C) 2016
//		File: 		   avian/include/stack.h
//		Purpose:		   Header file for stack implementation
// ======================================================================== */

#include <stdlib.h>

typedef struct __STACK
{
	size_t size;
	size_t max;
	byte* data;
	
} stack;

void push(stack*, byte);
void push_str(stack*,char[]);
byte pop(stack*);
byte peek(stack*);
bool empty(stack*);
bool full(stack*);
stack* new_stack(const size_t);
void delete_stack(stack*);
char* stack_str(stack*);

#endif