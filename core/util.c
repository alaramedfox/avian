#define UTIL_C_SOURCE
#include <util.h>
#include <stack.h>
#include <envar.h>
#include <stdio.h>
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

static char bytes_magnitude[] = "BKMGTP";

char* itoa_bytes(int32_t number)
{
	int magnitude=0;
	int32_t memsize=number;
	while(number > 1024) {
		number = number/1024;
		magnitude++;
	}
	
	stack* mem = new_stack(8);
	push_str(mem,itoa(number,10));
	push(mem,bytes_magnitude[magnitude]);
	char* str = stack_str(mem);
	delete_stack(mem);
	return str;
}

char* itoa(int number, int base)
{
	int row = vga_getrow();
	if(ENVAR.FLAGS.meminfo) { vga_movexy(row,3); print("itoa()\n"); }
	if(base < 1 || base > 16) return "Error: Invalid base";

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

	if(ENVAR.FLAGS.meminfo) { vga_movexy(row+1,3); print("new_stack()\n"); }
	stack* inverted = new_stack(result->size);
	if(ENVAR.FLAGS.meminfo) { vga_movexy(row+1,3); print("           \n"); }
	
	while(!empty(result)) {
		push(inverted,pop(result));
	}
	
	char* str_result = stack_str(inverted);
	delete_stack(inverted);
	delete_stack(result);
	
	if(ENVAR.FLAGS.meminfo) { 
		vga_movexy(row,3); 
		print("      "); 
	}
	return str_result;
}
