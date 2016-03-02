#define UTIL_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/util.h
 *		Purpose:	Provide misc functions
 */
#include <stack.h>
#include <util.h>
#include <string.h>
#include <vga.h>

/* 
 *	Debug assertion command - 
 *  Prints a message, the intended value, and the
 *	 actual value produced.
 */
void ASSERT(const char message[], int intend, int actual, base_t base)
{
/*
	print_time();
	vga_setcolor(C_WARN);
	print("INFO: ");
	print(message);
	char* intend_str = itoa(intend,base);
	char* actual_str = itoa(actual,base);
	int justify = strlen(intend_str) + strlen(actual_str) + 7;
	
	vga_movexy(vga_getrow(), 80-justify-1);
	
	print(" [ ");
	print(intend_str); 
	print(", "); print(actual_str); print(" ]\n");
	vga_setcolor(0x07);
*/
}

void print_time(void)
{
	print("[ ");
	vga_setcolor(0x02);
	print(itoa(clock(),DEC));
	vga_setcolor(0x07);
	print(" ] ");
}

void notify(const char str[])
{
	print_time();
	print(str);
}

static const char spin_chars[] = { '|', '/', '-', '\\', };
static int spin_state=0;
static bool waiting=false;
static int loc;
static char ch;

void wait_spin(bool done)
{
	if(!done && !waiting) {
		waiting = true;
		loc = vga_getloc();
		ch = vga_char_at(0,0);
		vga_color(0x05);
	}
	else if(!done && waiting) {
		vga_movexy(0,0);
		
		switch(spin_state)
		{
			case 0:    vga_write(spin_chars[0]); break;
			case 2000: vga_write(spin_chars[1]); break;
			case 4000: vga_write(spin_chars[2]); break;
			case 6000: vga_write(spin_chars[3]); break;
		}
		spin_state++;
		if(spin_state >= 8000) spin_state = 0;
	}
	else {
		waiting = false;
		vga_movexy(0,0);
		vga_write(ch);
		vga_moveptr(loc);
	}
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
	if(base == BYTES) {
		return itoa_bytes(number);
	}
	else if(base == BOOLEAN) {
		if(number == 0) return "false";
		else return "true";
	}
	else if(base < 1 || base > 16) {
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
	
	while(number != 0) {
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
	
	
	char* str_result = stack_str(inverted);
	delete_stack(inverted);
	delete_stack(result);
	
	return str_result;
}
