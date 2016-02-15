#define UTIL_C_SOURCE
#include <util.h>
#include <stack.h>
/*
 *		Avian Project
 *			File:		util.h
 *			Purpose:	Provide essential core functions
 */
 
 
/* Calculate the size of a null-terminated string literal */
#define C_STR_MAX_SIZE	512
size_t strlen(const char* str)
{
	size_t size=0;
	while(str[size] != '\0' && size < C_STR_MAX_SIZE) {
		++size;
	}
	return size;
}

enum __BASE
{
	DEC=10, HEX=16, OCT=8, BIN=2,
};

static char place_value[] = "0123456789ABCDEF";

char* itoa(int number, int base)
{
	if(base < 1 || base > 16) return "Error: Invalid base";

	stack* result = new_stack(16);
	bool negative=false;
	/* Handle 0 */
	if (number == 0) {
		delete_stack(result);
		return "0";
	}
	else if (number < 0){
		negative = true;
		number = number * -1;
	}

	// Process individual digits
	while (number != 0) {
		int i = number % base;
		push(result, place_value[i]);
		number = number/base;
	}

	if(base == 16) {
		push(result, 'x');
		push(result, '0');
	}
		else if(base == 10 && negative) {
		push(result, '-');
	}

	stack* inverted = new_stack(result->size);
	while(!empty(result)) {
		push(inverted,pop(result));
	}
	delete_stack(result);
	return stack_str(inverted);
}
