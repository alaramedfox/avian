#define KEYBOARD_C_SOURCE
#include <keyboard.h>
#include <pic.h>
#include <vga.h>
#include <util.h>
#include <idt.h>
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/keyboard.c
 *		Purpose:	Houses the keyboard interrupt function,
 *					and handles input from the keyboard.
 */
 
/* Create character buffer */
stack * stdin;

/* Define the keyboard map */ 
struct __KEYMAP KEYMAP = {
	.lowercase = {
		'\0',
		ESC,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', /* 14 */
		'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* 28 */
		CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`',  LSHIFT, '\\', /* 43 */
	   		'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', /* 53 */
		RSHIFT, '*', ALT, ' ', CAPS, /* 58 */
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM, SCR, HOME, 
		UARROW, PGUP, '-', LARROW, '5', RARROW, '+', END, DARROW, PGDN, INS, DEL,
		F11, F12, LGUI, RGUI, APPS 
	},
	.uppercase = {
		'\0',
		ESC,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
		'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
		CTRL, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', LSHIFT, '|',
		      'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 
		RSHIFT, '*', ALT, ' ', CAPS, 
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM, SCR, HOME, 
		UARROW, PGUP, '-', LARROW, '5', RARROW, '+', END, DARROW, PGDN, INS, DEL,
		F11, F12, LGUI, RGUI, APPS
	}
};

char* kb_buffer(void)
{
	return stack_str(stdin);
}


void kb_init(void)
{
	idt_add_handler((addr_t)keyboard_handler, IRQ_KEYBOARD);
	pic_enable_irq(IRQ_KEYBOARD);
	stdin = new_stack(128);
	
	print_time(); print("Started keyboard driver\n");
}

/* Special character handlidng */
void enter() 			{ push(stdin,'\n');	}
void backspace() 		{ pop(stdin); 	}
void undef_char() 	{ push(stdin,'?');	}

void keyboard_handler(void)
{
	byte status = inportb(KB_STATUS);
	word keycode;
	char key;
	
	//ASSERT("Caught keyboard IRQ",ENVAR.FLAGS.listen,status,HEX);
	
	/* Do not continue if system isn't listening */
	if(ENVAR.FLAGS.listen == false) { return; }

	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		ENVAR.FLAGS.keypress = true;
		keycode = inportb(KB_DATA);
		
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
				case '\0': 						break;
				default: push(stdin,key);	break;
			}
		}
	} /* if(status...) */
	pic_send_eoi(IRQ_KEYBOARD);
} /* extern void C_kb_driver(void) */
