#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

extern int8_t keyboard_map[128];
extern void keyboard_handler(void);

#include "idt.h"
#include "keydef.h"

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

/* Special character handlidng */
void enter() 			{ stdin_push('\n'); }
void backspace() 		{ stdin_pop(); }
void tab() 				{ for(int i=0;i<5;i++) stdin_push(' '); }
void undef_char() 	{ stdin_push('?'); }

void keyboard_driver(void)
{
	int8_t status = read_port(KEYBOARD_STATUS_PORT);
	int8_t keycode;

	/* write EOI */
	write_port(0x20, 0x20);

	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		char key = keyboard_map[(char) keycode];
		if(keycode < 0) return;
		if(MEMORY.FLAGS.stdin == false) return; //Ignore input if kernel isn't polling
		
		switch(key)
		{
			case '\n': enter(); 		break;
			case '\b': backspace(); 		break;
			case '\t': tab(); 			break;
			case '0':  undef_char(); 	break;
			default:   stdin_push(key); 	break;
		}
	}
}
