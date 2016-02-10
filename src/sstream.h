#ifndef SSTREAM_H_INCLUDED
#define SSTREAM_H_INCLUDED
/*
 *		Popcorn Kernel
 *			File:		sstream.h
 *			Purpose:	stack modification of strings
 */
 
#define SS_DEFAULT_SIZE 64

class stringstream {
private:
	size_t size;
	size_t max_size;
	char *contents;
	
public:
	/* Constructors and destructors */
	stringstream();
	stringstream(size_t);
	~stringstream();
	
	/* Operators */
	stringstream operator=(stringstream);
	
	/* Data functions */
	size_t logical_size(void) { return size; }
	size_t physical_size(void) { return max_size; }
	
	/* Stack functions */
	void push(char);	//Adds a character to the end of the stack
	char pop(void);	//Removes and returns the last character
	char peek(void);	//Returns the last character without removing it
	bool empty(void);	//Checks if the stream is empty
	bool full(void);  //Checks if stream is full
	void clear(void);	//Empties the stream and purges its data
	
	string str(void);	//Returns contents of stream as a string
};

stringstream::stringstream()
{
	size = 0;
	max_size = SS_DEFAULT_SIZE;
	contents = (char*) malloc(SS_DEFAULT_SIZE);
}

stringstream::stringstream(size_t init_size)
{
	size = 0;
	max_size = init_size;
	contents = (char*) malloc(init_size);
}

stringstream::~stringstream()
{
	free(contents);
}

stringstream stringstream::operator=(stringstream stream)
{
	free(contents);
	contents = (char*) malloc(stream.physical_size());
	max_size = stream.physical_size();
	size = stream.logical_size();
	for(count_t i=0; i<stream.logical_size(); i++)
	{
		contents[i] = stream.contents[i];
	}
	return *this;
}

bool stringstream::empty()
{
	return size == 0;
}

bool stringstream::full()
{
	return size == max_size;
}

void stringstream::clear()
{
	for(count_t i=0; i<max_size; i++) {
		contents[i] = '\0';
	}
	free(contents);
	contents = (char*) malloc(max_size);
	size = 0;
}

void stringstream::push(char c)
{
	if(!full()) {
		contents[size] = c;
		size++;
	}
	else contents[size-1] = c;
}

char stringstream::pop()
{
	if(!empty()) {
		char c = peek();
		size--;
		contents[size] = '\0';
		return c;
	}
	else return '\0';
}

char stringstream::peek()
{
	return contents[size-1];
}

string stringstream::str()
{
	return string(contents);
}



#endif