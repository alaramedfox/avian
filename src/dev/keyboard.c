#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64



#ifndef CORE_H_INCLUDED
#error "Keyboard driver requires system core"
#endif

#ifndef KEYDEF_H_INCLUDED
#error "Keyboard driver reuires the keydef.h header"
#endif

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	__write_port(0x21 , 0xFD);
}

/* Special character handlidng */
void enter() 			{ stdin.push('\r'); }
void backspace() 		{ stdin.pop(); }
void undef_char() 	{ stdin.push('?'); }

extern "C" void keyboard_driver(void)
{
	/* Do not continue if system isn't listening */
	if(ENVAR.FLAGS.listen == false) { return; }
	byte status = __read_port(KEYBOARD_STATUS_PORT);
	int16_t keycode;
	char key;
	/* write EOI */
	__write_port(0x20, 0x20);

	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		ENVAR.FLAGS.keypress = true;
		keycode = __read_port(KEYBOARD_DATA_PORT);
		
		/* Consider keycodes */
		if(keycode == LSHIFT_RELEASE_CODE || keycode == RSHIFT_RELEASE_CODE) {
			ENVAR.FLAGS.shift = false;
		}
		else if(keycode == LSHIFT_PRESS_CODE || keycode == RSHIFT_PRESS_CODE) {
			ENVAR.FLAGS.shift = true;
		}
		else if(keycode == CAPS_PRESS_CODE) {
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
				default:   stdin.push(key); 	break;
			}
		}
		else {
			
		}
		
	}
}