
const char HLINE1 = 196;
const char HLINE2 = 205;
const char HLINE3 = 240;
const char VLINE1 = 179;
const char VLINE2 = 186;
const char BLOCK1 = 176;
const char BLOCK2 = 177;
const char BLOCK3 = 178;
const char BLOCK4 = 219;

enum __EXTENDEDKEYS 
{
	ESC, ALT, CAPS, CTRL, LSHIFT, RSHIFT, F1, F2, F3, F4, F5, F6, F7, F8,
	F9, F10, F11, F12, NUM, SCR, HOME, UARROW, DARROW, LARROW, RARROW,
	PGUP, PGDN, END, INS, DEL, LGUI, RGUI, APPS,
};

#define LSHIFT_PRESS_CODE		42
#define LSHIFT_RELEASE_CODE	-86 //191

#define RSHIFT_PRESS_CODE		54
#define RSHIFT_RELEASE_CODE	-74

#define CAPS_PRESS_CODE		11

struct __KEYMAP 
{
	unsigned char lowercase[93];
	unsigned char uppercase[93];
};

struct __KEYMAP KEYMAP = {
	.lowercase = {
		'\0',
		ESC,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
		'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
		CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`',  LSHIFT, '\\', 
	   		'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 
		RSHIFT, '*', ALT, ' ', CAPS, 
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
