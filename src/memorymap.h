/*
 *		Popcorn Kernel
 *			File:		memorymap.h
 *			Purpose:	Maps and manages the core internal memory
 */
 

#define VRAM_START	0xb8000				//Actual address of system VRAM
#define VRAM_LENGTH	80*24*2				//Length of VRAM in bytes
#define VRAM_CHARS	VRAM_LENGTH/2		//Length of VRAM in words

#define CHAR_NEWLINE 0xA					//Define the newline char '\n'
#define CHAR_TAB	 0x9						//Define the tab char '\t'



#define	NOECHO	1
#define	ECHO		2
#define	PASSWD	3

struct __GLOBAL {
	color_t	color;						//Global standard background color
	byte	   echostate;					//Whether or not to echo STDIN
	byte	   tabsize;						//Size of the tabs
};

struct __FLAGS {
	bool 		listen;						//If set, signal to catch input
	bool 		repaint;						//If set, signal need to repaint
	bool		captain;						//If set, allow unrestricted access
	bool		raw;							//If set, any output is not escaped.
	bool		shift;						//If set, flag for the uppercase charset
	bool		caps;							//If set, permanant flag for uppercase
	bool		debugmode;					//For debugging stuff
	bool		keypress;					//True when key is pressed
	bool		general[16];				//General use flags
};

struct __MEMORY {
	__INDEX		INDEX;
	__FLAGS		FLAGS;
	__GLOBAL		GLOBAL;
	
	void init();
	
} MEMORY;

void __MEMORY::init()
{
	IO.vram 			= (byte*)0xb8000;
	IO.instream		= string();
	GLOBAL.color 	= 0x07;
	INDEX.vram	 	= 0;
	FLAGS.listen 	= false;
	FLAGS.repaint 	= false;
	FLAGS.keypress = false;
	GLOBAL.echostate = ECHO;
	FLAGS.captain = false;
	GLOBAL.tabsize = 4;
}