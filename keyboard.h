#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64


#define ENTER_KEY_CODE 0x1C

extern unsigned char keyboard_map[128];
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
	vram_this(' ');
	vram_next(this_color);
	if(vptr >= 2) { vptr-=2; }
}
inline void tab()
{
	print("     ");
}
inline void undef_char()
{
	print("^?");
}

void keyboard_driver(void)
{
	unsigned char status;
	char keycode;

	/* write EOI */
	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		unsigned char key = keyboard_map[(char) keycode];
		if(keycode < 0) return;
		if(key > 128) return;

		switch(key)
		{
		case '\n': newline(); break;
		case '\b': backspace(); break;
		case '\t': tab(); break;
		case '0': undef_char(); break;
		default: vram_over();
			    vram_this(key);
			    vram_next(this_color);
			    break;
		}
	}
}
