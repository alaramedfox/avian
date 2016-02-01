/*
 *		Popcorn Kernel
 *			File:		memorymap.h
 *			Purpose:	Maps and manages the core internal memory
 */
 
#define VRAM_C	80*2							//Define the number of char columns
#define VRAM_R	24*2							//Define the number of char rows
#define VGA_C	80								//Width of VGA in bytes
#define VGA_R	24								//Height of VGA in bytes

#define VRAM_START	0xb8000				//Actual address of system VRAM
#define VRAM_LENGTH	80*24*2				//Length of VRAM in bytes
#define VRAM_CHARS	VRAM_LENGTH/2		//Length of VRAM in words

#define CHAR_NEWLINE 0xA					//Define the newline char '\n'
#define CHAR_TAB	 0x9						//Define the tab char '\t'

#define THIS_COL	MEMORY.INDEX.stdout%(VGA_C)			//Current column in VRAM
#define THIS_ROW	MEMORY.INDEX.stdout/(VGA_C)			//Current row in VRAM

#define	NOECHO	1
#define	ECHO		2
#define	PASSWD	3

struct __IO {
	char 		stdin[256];					//The input stream buffer
	char*		stdout; 						//Access to system video RAM
};

struct __INDEX {
	int16_t	stdout;						//Output stream buffer pointer (vptr)
	size_t	stdin;						//Input stream size
};

struct __GLOBAL {
	int8_t	color;						//Global standard background color
	int8_t	echostate;					//Whether or not to echo STDIN
	int8_t	tabsize;						//Size of the tabs
};

struct __FLAGS {
	bool 		stdin;						//If set, signal to catch input
	bool 		stdout;						//If set, signal need to repaint
	bool		raw;							//If set, any output is not escaped.
	bool		shift;						//If set, flag for the uppercase charset
	bool		caps;							//If set, permanant flag for uppercase
	bool		debugmode;					//For debugging stuff
};

struct __MEMORY {
	struct __IO			IO;
	struct __INDEX		INDEX;
	struct __FLAGS		FLAGS;
	struct __GLOBAL	GLOBAL;
};

struct __MEMORY MEMORY;

void memorymap_init()
{
	MEMORY.IO.stdout 		= (char*)0xb8000;
	MEMORY.GLOBAL.color 	= 0x07;
	MEMORY.INDEX.stdout 	= 0;
	MEMORY.INDEX.stdin 	= 0;
	MEMORY.FLAGS.stdin 	= false;
	MEMORY.FLAGS.stdout 	= false;
	MEMORY.GLOBAL.echostate = ECHO;
	MEMORY.GLOBAL.tabsize = 4;
}