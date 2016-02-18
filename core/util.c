#define UTIL_C_SOURCE
/*
 *		Avian Project
 *			File:		util.h
 *			Purpose:	Provide essential core functions
 */
#include <stack.h>
#include <util.h>
 
/* Calculate the size of a null-terminated string literal */
size_t strlen(const char* str)
{
	size_t size=0;
	while(str[size] != '\0') {
		++size;
	}
	return size;
}

static const char place_value[] = "0123456789ABCDEF";
static const char bytes_magnitude[] = "BKMGTP";

char* itoa_bytes(int number)
{
	int magnitude = 0;
	int memsize = number;
	stack* mem = new_stack(8);
	
	while(number > 1024) {
		number = number/1024;
		magnitude++;
	}
	
	push_str(mem,itoa(number,10));
	push(mem,bytes_magnitude[magnitude]);
	char* str = stack_str(mem);
	delete_stack(mem);
	return str;
}

char* itoa(int number, base_t base)
{
	if(base < 1 || base > 16) {
		return "";
	}

	stack* result = new_stack(16);
	bool negative=false;
	/* Handle 0 */
	if (number == 0) {
		delete_stack(result);
		return "0";
	}
	if (number < 0){
		negative = true;
		number = number * -1;
	}

	// Process individual digits
	
	until(number == 0) {
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
	
	stack* inverted = new_stack(result->size);\
	until(empty(result)) {
		push(inverted,pop(result));
	}
	
	
	char* str_result = stack_str(inverted);
	delete_stack(inverted);
	delete_stack(result);
	
	return str_result;
}
