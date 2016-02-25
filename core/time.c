#define TIME_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/time.h
 *		Purpose:	Handle the PIT and time-related functions
 */
#include <time.h>
#include <pic.h>
#include <idt.h>
#include <util.h>
#include <vga.h>
#include <asmfunc.h>
#include <mmap.h>
#include <envar.h>

static volatile uint32_t tick=0;

enum __PIT_PORTS
{
	PIT_DAT0 = 0x40,
	PIT_DAT1 = 0x41,
	PIT_DAT2 = 0x42,
	PIT_MODE = 0x43,
};

void time_init(word frequency)
{
	/* Calculate the target frequency */
	word f = 1193180 / frequency;
	
	/* Send initialization command */
	outportb(PIT_MODE, 0x36);
	
	/* Split divisor into two bytes */
	byte low = (byte)(f & 0xFF);
	byte high = (byte)(f >> 8);
	
	/* Send the divisor data */
	outportb(PIT_DAT0, low);
	outportb(PIT_DAT0, high);
	
	idt_add_handler((addr_t)pit_irq, IRQ_PIT);
	pic_enable_irq(IRQ_PIT);
	
	print_time(); print("Started system clock\n");
	
}

uint32_t clock(void)
{
	return tick;
}

void sleep(uint32_t ms)
{
	uint32_t start = clock();
	while(clock()<start+ms);
	return;
}

static void print_memory(void)
{
	int loc = vga_getloc();
	vga_clear_row(1);
	print("Memory: "); print(itoa(mem_used(),BYTES));
	print(" / "); 		 print(itoa(mem_free(), BYTES));
	vga_moveptr(loc);
}


__attribute__ ((hot)) void pit_handler(void) 
{
	tick++;
	pic_send_eoi(IRQ_PIT);
}




