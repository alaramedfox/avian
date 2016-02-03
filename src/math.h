/*
 *	Popcorn Kernel
 *	  File:		math.h
 *	  Purpose:	Basic mathematics, like powers and such
 */
 
#define sqr(x)		x*x

//typedef unsigned int8_t byte;

/* Converts an integer into a char array */
string itos(int32_t number)
{
	bool NEGATIVE_FLAG = false;
	if(number < 0) 
	{
		number = number * -1;
		NEGATIVE_FLAG = true;
	}
	
	string temp;
	string str;
	
	while(number>0)
	{
		int digit = number%10;
		temp.push(digit+48);
		number = number/10;
	}
	if(NEGATIVE_FLAG)
	{
		temp.push('-');
	}
	for(size_t i=0; i<temp.size(); i++)
	{
		str.push( temp.pop() );
	}
	return str;
}

int strsum(string str)
{
	int sum=0;
	for(count_t i=0; i<str.size(); i++) {
		sum += str[i];
	}
	return sum;
}
