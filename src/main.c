/* 
 *	Popcorn Kernel
 *	  File:		main.c
 *	  Purpose:	Main loop, definitions, and primary entry point.
 *				This is the Popcorn start file.
 */
 
const char VERSION[] = "Popcorn Kernel v0.5.2";

#include <types.h>
#include <vga.h>
#include <envar.h>
#include <idt.h>
#include <keyboard.h>

void init(void) 
{
	ENVAR_init();		//Init global values
	//stdin = CIN();		//Init standard input stream
	//stdout = VGA();	//Init standard output stream (screen)
	idt_init();			//Init interrupt controller
	kb_init();			//Init keyboard driver
	//stdout.clear();	//Fill screen with blank spaces
	ENVAR.FLAGS.listen = true;
}

void bootscreen(void)
{
	vga_setcolor(0x07);
	vga_clear();
	for(int i=0; VERSION[i] != '\0'; i++)
	{
		vga_write(VERSION[i]);
		vga_increment();
	}
	/*
	stdout.setcolor(C_BLUESCR);
	for(size_t i=0; i<C.cols; i++) { stdout.addch(HLINE1); }
	for(size_t i=0; i<C.cols*4; i++) { stdout.addch(' '); }
	for(size_t i=0; i<C.cols; i++) { stdout.addch(HLINE1); }
	
	stdout.move(1,0);
	stdout.print("\tPopcorn Kernel v" VERSION "\n");
	stdout.print("\tGNU Public Liscense -- Bryan Webb, 2016\n");
	
	stdout.setcolor(C_TERMINAL);
	*/
}

void print(char str[])
{
	for(int i=0; str[i] != '\0'; i++) {
		vga_write(str[i]);
		vga_increment();
	}
}

extern void C_main(void) 
{
	init();
	bootscreen();
	vga_movexy(0,0);
	stack* stk = new_stack(50);
	push_str(stk, "HELLO THERE PERSON");
	//char* str = stk->data;
	vga_movexy(1,0);
	print(to_string(stk));
	//stdout.move(6,0);

	while(1) {
		vga_movexy(2,0);
		print(kb_buffer());
	}
}
