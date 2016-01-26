/* 
 *	Popcorn Kernel
 *	  File:		stdio.h
 *	  Purpose:	File and keyboard input and output handling
 */
 
#define STD_MAX	256			//Maximum characters for input 

int	 	STDIN_FLAG;

char 	std_stream[256];		//The input stream buffer
size_t	stdin_size=0;			//Size of buffer

char stdin_clear(void)
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
		return std_stream[stdin_size];
	} else {
		stdin_clear();
		return '\0';
	}
}
