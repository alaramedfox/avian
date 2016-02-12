#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED
/*
 *		Popcorn Kernel - Bryan Webb
 *		File: 		stack.h
 *		Purpose:		Header file for stack.c
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <mmap.h>

typedef struct __STACK stack;
void push(char, stack*);
char pop(stack*);
char peek(stack*);
bool empty(stack*);
bool full(stack*);
stack* new_stack(size_t);
void delete_stack(stack*);
char* to_string(stack*);

#endif