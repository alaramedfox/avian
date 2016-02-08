#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

extern "C" void keyboard_handler(void);

#include "idt.h"
#include "keydef.h"

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	__write_port(0x21 , 0xFD);
}

/* Special character handlidng */
void enter() 			{ stdin::push('\n'); }
void backspace() 		{ stdin::pop(); }
void tab() 				{ for(count_t i=0;i<5;i++) stdin::push(' '); }
void undef_char() 	{ stdin::push('?'); }

extern "C" //cdecl export
void keyboard_driver(void)
{
	/* Do not continue if system isn't listening */
	if(MEMORY.FLAGS.listen == false) { return; }
	byte status = __read_port(KEYBOARD_STATUS_PORT);
	int16_t keycode;
	char key;
	/* write EOI */
	__write_port(0x20, 0x20);

	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		MEMORY.FLAGS.keypress = true;
		keycode = __read_port(KEYBOARD_DATA_PORT);
		
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
		else if(keycode < 127) {
			if(MEMORY.FLAGS.shift) {
				key = KEYMAP.uppercase[keycode];
			}
			else {
				key = KEYMAP.lowercase[keycode];
			}
		
			switch(key)
			{
				case '\b': backspace(); 	break;
				case '0':  undef_char(); 	break;
			
				default:   MEMORY.IO.cin = key; 	break;
			}
		}
		else {
			
		}
		
	}
}
