#define KEYBOARD_C_SOURCE
#include <keyboard.h>
/*
 *		Popcorn Kernel - Bryan Webb
 *		File:		/core/keyboard.c
 *		Purpose:	Houses the keyboard interrupt function,
 *					and handles input from the keyboard.
 */


void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	ASM_write_port(0x21 , 0xFD);
}

/* Special character handlidng */
void enter() 			{  }
void backspace() 		{  }
void undef_char() 	{  }

extern void C_kb_driver(void)
{
	/* Do not continue if system isn't listening */
	if(ENVAR.FLAGS.listen == false) { return; }
	byte status = ASM_read_port(KB_STATUS_PORT);
	int16_t keycode;
	char key;
	/* write EOI */
	ASM_write_port(0x20, 0x20);

	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		ENVAR.FLAGS.keypress = true;
		keycode = ASM_read_port(KB_DATA_PORT);
		
		/* Consider keycodes */
		if(keycode == LSHIFT_UP || keycode == RSHIFT_UP) {
			ENVAR.FLAGS.shift = false;
		}
		else if(keycode == LSHIFT_DN || keycode == RSHIFT_DN) {
			ENVAR.FLAGS.shift = true;
		}
		else if(keycode == CAPS_DN) {
			ENVAR.FLAGS.caps = !ENVAR.FLAGS.caps;
		}
		else if(keycode < 127) {
			if(ENVAR.FLAGS.shift) {
				key = KEYMAP.uppercase[keycode];
			}
			else {
				key = KEYMAP.lowercase[keycode];
			}
		
			switch(key)
			{
				case '\b': backspace(); 	break;
				case '0':  undef_char(); 	break;
				case '\0': break;
				default: 	break;
			}
		}
		else {
			
		}
		
	}
}
