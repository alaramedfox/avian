#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED
/*
 *		Avian Project - Bryan Webb
 *		File: 		stack.h
 *		Purpose:		Header file for stack.c
 */

#include <types.h>
#include <stdlib.h>

typedef struct __STACK
{
	size_t size;
	size_t max;
	char* data;
} stack;

void push(stack*, char);
void push_str(stack*,char[]);
char pop(stack*);
char peek(stack*);
bool empty(stack*);
bool full(stack*);
stack* new_stack(const size_t);
void delete_stack(stack*);
char* stack_str(stack*);

#endif