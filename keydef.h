/* The following array is taken from 
    http://www.osdever.net/bkerndev/Docs/keyboard.htm
   All credits where due
*/

#define CTRL 	128
#define LSHIFT	129
#define RSHIFT	130
#define ALT	131
#define CAPS	132
#define F1	133
#define F2	134
#define F3	135
#define F4	136
#define F5	137
#define F6	138
#define F7	139
#define F8	140
#define F9	141
#define F10	142
#define NUM	143
#define SCR	144
#define HOME	145
#define UARROW	146
#define PGUP	147
#define LARROW	148
#define RARROW	149
#define END	150

int8_t shifted_map[128] =
{
	 0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'L', 'L', 'L', ':', '"',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
};

int8_t keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    CTRL,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   LSHIFT,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   RSHIFT,				/* Right shift */
  '*',
    ALT,	/* Alt */
  ' ',	/* Space bar */
    CAPS,	/* Caps lock */
    F1,	/* 59 - F1 key ... > */
    F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9,
    F10,	/* < ... F10 */
    NUM,	/* 69 - Num lock*/
    SCR,	/* Scroll Lock */
    HOME,	/* Home key */
    UARROW,	/* Up Arrow */
    PGUP,	/* Page Up */
  '-',
    LARROW,	/* Left Arrow */
    0,
    RARROW,	/* Right Arrow */
  '+',
    END,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
