#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64


#define ENTER_KEY_CODE 0x1C

extern int8_t keyboard_map[128];
extern void keyboard_handler(void);

#include "idt.h"
#include "keydef.h"

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

inline void newline(void)
{
	move(THIS_ROW+1,0);
}

inline void backspace()
{
	stdin_pop();
}
inline void tab()
{
	stdin_push("     ");
}
inline void undef_char()
{
	stdin_push('?');
}

void keyboard_driver(void)
{
	int8_t status = read_port(KEYBOARD_STATUS_PORT);
	char keycode;
	
	//STDIN_FLAG = 1;

	/* write EOI */
	write_port(0x20, 0x20);

	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		char key = keyboard_map[(char) keycode];
		if(keycode < 0) return;

		switch(key)
		{
		case '\n': newline(); break;
		case '\b': backspace(); break;
		case '\t': tab(); break;
		case '0':  undef_char(); break;
		default: 	 stdin_push(key);
			    break;
		}
	}
}
