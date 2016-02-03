/*
 *	Popcorn Kernel
 *	  File:		math.h
 *	  Purpose:	Basic mathematics, like powers and such
 */
 
#define sqr(x)		x*x

//typedef unsigned int8_t byte;

/* Converts an integer into a char array */
string itos(int32_t i)
{
	bool NEGATIVE_FLAG = false;
	if(i < 0) 
	{
		i = i*-1;
		NEGATIVE_FLAG = true;
	}
	
	string temp = {""};
	static string str;
	size_t str_size = 0;
	while(i>0)
	{
		int digit = i%10;
		temp[str_size] = digit+48;
		i = i/10;
		str_size++;
		if(str_size == 6) break;
	}
	if(NEGATIVE_FLAG)
	{
		temp[str_size++] = '-';
	}
	for(size_t j=0; j<str_size; j++)
	{
		str[j] = temp[str_size-j-1];
	}
	return str;
}
