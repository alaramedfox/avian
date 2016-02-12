/* 
 *	Popcorn Kernel
 *	  File:		main.c
 *	  Purpose:	Main loop, definitions, and primary entry point.
 *				This is the Popcorn start file.
 */
 
const char VERSION[] = "0.5.2";

#include <types.h>

void init(void) 
{
	ENVAR_init();		//Init global values
	stdin = CIN();		//Init standard input stream
	stdout = VGA();	//Init standard output stream (screen)
	idt_init();			//Init interrupt controller
	kb_init();			//Init keyboard driver
	stdout.clear();	//Fill screen with blank spaces
}

void bootscreen(void)
{
	stdout.setcolor(C_BLUESCR);
	for(size_t i=0; i<C.cols; i++) { stdout.addch(HLINE1); }
	for(size_t i=0; i<C.cols*4; i++) { stdout.addch(' '); }
	for(size_t i=0; i<C.cols; i++) { stdout.addch(HLINE1); }
	
	stdout.move(1,0);
	stdout.print("\tPopcorn Kernel v" VERSION "\n");
	stdout.print("\tGNU Public Liscense -- Bryan Webb, 2016\n");
	
	stdout.setcolor(C_TERMINAL);
}

extern void C_main(void) 
{
	init();
	bootscreen();
	
	stdout.move(6,0);

	while(1) {
		stdout.print("\n> ");
		char input[] = stdin.scan();
		stdout.print("(");
		stdout.print(input);
		stdout.print(")");
	}
}
