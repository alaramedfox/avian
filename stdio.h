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

int	 	STDIN_FLAG;

char 	std_stream[256];		//The input stream buffer
size_t	stdin_size=0;			//Size of buffer

void stdin_clear();
void stdin_push(char);
char stdin_pop();
char stdin_peek();

char* stdin(int8_t FLAG)
{
	stdin_clear();
	int16_t location = vptr;
	//i_listen();
	while(stdin_peek() != ENTER_KEY_CODE)
	{
		move(4,0);
		print("> ");
		if(FLAG == ECHO)
		{
			//vmove(location);
			
			print(std_stream); //Command echo
			print("_ ");
		}
	}
	//i_ignore();
	return std_stream;
}

void stdin_clear(void)
{
	std_stream[0] = '\0';
	stdin_size=0;
}

void stdin_push(char c)
{
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
