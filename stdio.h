/* 
 *	Popcorn Kernel
 *	  File:		stdio.h
 *	  Purpose:	File and keyboard input and output handling
 */
 
#define STD_MAX	256			//Maximum characters for input 
#define ENTER_KEY_CODE 0x1C

#define	NOECHO	1
#define	ECHO		2
#define	PASSWD	3

extern void i_listen();			//Declare the listener in the ASM file
extern void i_ignore();			//Declare the un-listener in ASM

bool	 	STDIN_FLAG=0;			//0 = no update, 1 = update

char 	std_stream[256];		//The input stream buffer
size_t	stdin_size=0;			//Size of buffer

void stdin_clear();
void stdin_push(char);
char stdin_pop();
char stdin_peek();

char* stdin(int8_t FLAG)
{
	VUPDATE = true;	//Flag for repaint initially
	stdin_clear();
	int16_t location = vptr;
	int32_t counter = 0;
	bool blink = true;
	while(stdin_peek() != '\n')
	{
		vmove(location);
		
		if(VUPDATE)	//Only update screen if a key was pressed
		{
			switch(FLAG)
			{
				case ECHO: print(std_stream); break;
				case PASSWD: for(size_t i=0;i<stdin_size;i++)
					    { print("*"); } break;
				case NOECHO: /* Implement */	break;
				default: print(std_stream); 	break;
			}
			VUPDATE = false; //De-flag the update flag
		}
		vmove(location+stdin_size);
		blink?print("_ "):print("  ");
		if(counter > 65534*3) { counter = 0; blink = !blink; }
		counter++;
	}
	stdin_clear();
	return std_stream;
}

void stdin_clear(void)
{
	VUPDATE = true;
	std_stream[0] = '\0';
	stdin_size=0;
}

void stdin_push(char c)
{
	VUPDATE = true;
	if(stdin_size < STD_MAX)			
	{
		std_stream[stdin_size] = c;
		std_stream[stdin_size+1] = '\0';
		stdin_size++;
	} 
	else 
	{
		stdin_push(STD_MAX-1);
	}
}

char stdin_pop(void)
{
	VUPDATE = true;
	if(stdin_size > 0)
	{
		stdin_size--;
		std_stream[stdin_size] = '\0';
		return std_stream[stdin_size-1];
	} else {
		stdin_clear();
		return '\0';
	}
}

char stdin_peek(void)
{
	if(stdin_size > 0)
	{
		return std_stream[stdin_size-1];
	} else {
		return '\0';
	}
}
