/*
 *		Popcorn Kernel
 *			File:		string.h
 *			Purpose:	string-related functions
 */
 
typedef char*  string;
 
bool strcomp(const string a, const string b)
{
	int i;
	for(i=0; a[i] == b[i]; i++) {
		if(a[i] == '\0' || b[i] == '\0') break;
	}
   
   if(a[i]=='\0' && b[i]=='\0') {
    	return true;
	}
	else {
		return false;
	}
}

int8_t strwordcount(const string str)
{
	int8_t count=1;
	for(int8_t i=0; str[i] != '\0'; i++) {
		if(str[i] == ' ') count++;
	}
	return count;
}

string strword(const string str, int8_t index) {
	static string word;
	int8_t current_index=0;
	int8_t word_at=0;
	
	for(int8_t i=0; str[i] != '\0'; i++) {
		if(str[i] == ' ') {	//Start of new word
			word_at++;
			i++; //Skip the space so it doesn't appear in the string
		}
		if(word_at == index) {	//Found target word, start copying into new array
			word[current_index] = str[i];
			current_index++;
		}
	
		
	}
	
	word[current_index] = '\0'; //Null-terminate string
	return word;
}

