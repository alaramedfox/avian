/*
 *		Popcorn Kernel
 *			File:		string.h
 *			Purpose:	Define a string object
 */

class string {
private:
	size_t length;
	char contents[];

public:
	/* Constructors */
	string();
	
	/* Operators */
	bool operator==(string);
	char operator[](size_t);
	string operator=(const char[]);
	
	/* Data */
	size_t size();
	string word(size_t);
	size_t wordcount();
	
	/* Stack functions */
	void push(char);
	char pop();
	char peek();
	
};

void string::push(char c)
{
	contents[length] = c;
	length++;
}

char string::pop()
{
	length--;
	char c = contents[length];
	contents[length] = '\0';
	return c;
}

char string::peek()
{
	return contents[length-1];
}
 
bool string::operator==(string str)
{
	if(str.size() != length) return false;
	for(int i=0; i<str.size() && i<length; i++)
	{
		if(str[i] != contents[i]) return false;
		else continue;
	}
	return true;
}

size_t string::size()
{
	return length;
}

char string::operator[](size_t index)
{
	if(index > length-1) return '\0';
	else return contents[index];
}



size_t string::wordcount()
{
	int count=1;
	for(int i=0; i < length; i++) {
		if(contents[i] == ' ') { count++; }
	}
	return count;
}

string string::word(size_t target_index)
{
	string word;
	size_t word_index=0;
	
	for(size_t i=0; i < length; i++) {
		
		if(contents[i] == ' ') {
			word_index++;
		}
		else if(word_index == target_index) {
			word.push(contents[i]);
		}
		
	}
	
	return word;
}

