#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

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
	int16_t keycode;
	char key;
	/* write EOI */
	write_port(0x20, 0x20);

	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(MEMORY.FLAGS.debugmode) {
			newline();
			print(itos(keycode)); 
		}
		
		/* Consider keycodes */
		if(keycode == LSHIFT_RELEASE_CODE || keycode == RSHIFT_RELEASE_CODE) {
			MEMORY.FLAGS.shift = false;
		}
		else if(keycode == LSHIFT_PRESS_CODE || keycode == RSHIFT_PRESS_CODE) {
			MEMORY.FLAGS.shift = true;
		}
		else if(keycode == CAPS_PRESS_CODE) {
			MEMORY.FLAGS.caps = !MEMORY.FLAGS.caps;
		}
		else if(keycode > 0) {
			if(MEMORY.FLAGS.shift) {
				key = KEYMAP.uppercase[keycode];
			}
			else {
				key = KEYMAP.lowercase[keycode];
			}
		
			switch(key)
			{
				case '\n': enter(); 		   break;
				case '\b': backspace(); 	break;
				case '0':  undef_char(); 	break;
			
				default:   stdin_push(key); 	break;
			}
		}
		else {
			
		} 
		
	}
}
