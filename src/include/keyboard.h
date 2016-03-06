#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED
// ======================================================================== */
//		Avian Kernel   Bryan Webb
//		File:		      avian/drivers/keyboard.h
//		Purpose:	      Header for keyboard driver
// ======================================================================== */
 
#include <stack.h>
#include <asmfunc.h>

char* kb_buffer(void);
void kb_init(void);
void kb_enter(void);
void kb_backspace(void);
void kb_undef_char(void);

void keyboard_handler(void);

enum __EXTENDED_CHARS
{
	HLINE1=196, HLINE2=205, HLINE3=240,
	VLINE1=179, VLINE2=186,
	BLOCK1=176, BLOCK2=177, BLOCK3=178, BLOCK4=219,
};

enum __EXTENDED_KEYS 
{
	ESC, ALT, CAPS, CTRL, LSHIFT, RSHIFT, F1, F2, F3, F4, F5, F6, F7, F8,
	F9, F10, F11, F12, NUM, SCR, HOME, UARROW, DARROW, LARROW, RARROW,
	PGUP, PGDN, END, INS, DEL, LGUI, RGUI, APPS,
};

enum __SPECIAL_KEYS
{
	LSHIFT_DN=42, LSHIFT_UP=170,
	RSHIFT_DN=54, RSHIFT_UP=182,
	CAPS_DN=11,
};

struct __KEYMAP 
{
	unsigned char lowercase[93];
	unsigned char uppercase[93];
};


#endif