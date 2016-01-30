/* The following array is taken from 
    http://www.osdever.net/bkerndev/Docs/keyboard.htm
   All credits where due
*/

const char HLINE1 = 196;
const char HLINE2 = 205;
const char HLINE3 = 240;
const char VLINE1 = 179;
const char VLINE2 = 186;
const char BLOCK1 = 176;
const char BLOCK2 = 177;
const char BLOCK3 = 178;
const char BLOCK4 = 219;

#define ESC		1
#define CTRL 	'0' //128
#define LSHIFT	'0' //129
#define RSHIFT	'0' //130
#define ALT		'0' //131
#define CAPS	'0' //132
#define F1		'0' //133
#define F2		'0' //134
#define F3		'0' //135
#define F4		'0' //136
#define F5		'0' //137
#define F6		'0' //138
#define F7		'0' //139
#define F8		'0' //140
#define F9		'0' //141
#define F10		'0' //142
#define NUM		'0' //143
#define SCR		'0' //144
#define HOME	'0' //145
#define UARROW	'0' //146
#define PGUP	'0' //147
#define LARROW	'0' //148
#define RARROW	'0' //149
#define END		'0' //150
#define PGDN	'0' //151
#define INS		'0' //152
#define DEL		'0' //153
#define DARROW '0' //154
#define F11		'0' //155
#define F12		'0' //156
#define LGUI	'0' //157
#define RGUI	'0' //158
#define APPS	'0' //159

#define LSHIFT_PRESS_CODE		42
#define LSHIFT_RELEASE_CODE	-86 //191

struct __KEYMAP {
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
