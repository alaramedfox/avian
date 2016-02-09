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
	char *contents;

public:
	/* Constructors */
	string();
	string(const char*);
	~string();
	
	/* Operators */
	bool operator==(string);
	char operator[](size_t);
	string operator=(char*);
	string operator=(string);
	string operator+(string);
	string operator+(char*);
	
	/* Data */
	size_t size();
	string getword(size_t);
	size_t wordcount();
	
	
};

string::string()
{
	length = 0;
	contents = (char*)malloc(0);
}

string::~string()
{
	free(contents);
}

string::string(const char* c)
{
	length = strlen(c)-1;
	contents = (char*)malloc(length);
	for(count_t i=0; i<strlen(c); i++) {
		contents[i] = c[i];
	}
}

string string::operator=(char* c_str)
{
	return string(c_str);
}

string string::operator=(string cpp_str)
{
	return string(cpp_str);
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
			//w.push(contents[i]);
		}
		
	}
	
	return w;
}

#endif