#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

/*
 *		Popcorn Kernel
 *			File:		string.h
 *			Purpose:	Define a string object
 */

class string {
private:
	size_t length;
	char* contents;

public:
	/* Constructors */
	string();
	string(const char*);
	
	/* Operators */
	bool operator==(string);
	char operator[](size_t);
	string operator=(char*);
	string operator=(string);
	string operator+(string);
	string operator+(char*);
	
	/* Data */
	void clear();
	size_t size();
	string getword(size_t);
	size_t wordcount();
	
	/* Stack functions */
	void push(char);
	char pop();
	char peek();
	
};

string::string()
{
	length=0;
}

string::string(const char* c)
{
	for(count_t i=0; c[i] != '\0'; i++) {
		push(c[i]);
	}
}

void string::clear()
{
	length = 0;
}

void string::push(char c)
{
	contents[length] = c;
	length++;
}

char string::pop()
{
	char c = contents[length-1];
	contents[length-1] = '\0';
	length--;
	return c;
}

char string::peek()
{
	return contents[length-1];
}

string string::operator+(string str)
{
	for(size_t i=0; i<str.size(); i++) {
		this->push(str[i]);
	}
	return *this;
}

string string::operator+(char* c_str)
{
	for(size_t i=0; c_str[i] != '\0'; i++) {
		this->push(c_str[i]);
	}
	return *this;
}

string string::operator=(char* c_str)
{
	string str;
	for(count_t i=0; c_str[i] != '\0'; i++) {
		str.push(c_str[i]);
	}
	return str;
}

string string::operator=(string cpp_str)
{
	string str;
	for(count_t i=0; i<cpp_str.size(); i++) {
		str.push(cpp_str[i]);
	}
	return str;
}
 
bool string::operator==(string str)
{
	if(str.size() != length) return false;
	for(count_t i=0; i<str.size() && i<length; i++)
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
	if(index > length-1) return '!';
	else return contents[index];
}



size_t string::wordcount()
{
	int count=1;
	for(count_t i=0; i < length; i++) {
		if(contents[i] == ' ') { count++; }
	}
	return count;
}

string string::getword(size_t target_index)
{
	string w;
	size_t word_index=0;
	
	for(size_t i=0; i < length; i++) {
		
		if(contents[i] == ' ') {
			word_index++;
		}
		if(word_index == target_index) {
			w.push(contents[i]);
		}
		
	}
	
	return w;
}

#endif