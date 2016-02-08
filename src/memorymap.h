/*
 *		Popcorn Kernel
 *			File:		memorymap.h
 *			Purpose:	Maps and manages the core internal memory
 */
 

#define CHAR_NEWLINE 0xA					//Define the newline char '\n'
#define CHAR_TAB	 0x9						//Define the tab char '\t'



#define	NOECHO	1
#define	ECHO		2
#define	PASSWD	3

struct __GLOBAL {
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
	__FLAGS		FLAGS;
	__GLOBAL		GLOBAL;
	
	void init();
	
} MEMORY;

void __MEMORY::init()
{
	FLAGS.listen 	= false;
	FLAGS.repaint 	= false;
	FLAGS.keypress = false;
	GLOBAL.echostate = ECHO;
	FLAGS.captain = false;
	GLOBAL.tabsize = 4;
}