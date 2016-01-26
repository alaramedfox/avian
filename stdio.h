/* 
 *	Popcorn Kernel
 *	  File:		stdio.h
 *	  Purpose:	File and keyboard input and output handling
 */
 
#define STD_MAX	128	//Maximum characters for input 

int	 	STDIN_FLAG;

char 	stdin[STD_MAX];	//The input stream buffer
size_t	stdin_size=0;		//Size of buffer
char 	str[STD_MAX];

char* stdin_get(void)
{
	for(size_t i=0; i<stdin_size; i++)
	{
		str[i] = stdin[i];
	}
	str[stdin_size] = 'E';
	STDIN_FLAG = 0;
	return str;
}

void stdin_shift()
{
	for(size_t i=0; i<STD_MAX-1; i++)
	{
		stdin[i] = stdin[i+1];
	}
	//STDIN_FLAG = 0;
}

void stdin_push(char c)
{
	if(stdin_size < STD_MAX)
	{
		stdin_size++;
		stdin[stdin_size] = 'T';
	} 
	else 
	{
		//stdin_shift();
		stdin_push(STD_MAX-1);
	}
	STDIN_FLAG = 1;
}

char stdin_pop(void)
{
	if(stdin_size > 0)
	{
		stdin_size--;
		STDIN_FLAG = 1;
		return stdin[stdin_size];
	} else {
		return '\0';
	}
}
