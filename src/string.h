/*
 *		Popcorn Kernel
 *			File:		string.h
 *			Purpose:	string-related functions
 */
 
bool strcomp(const char a[], const char b[])
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

char* strword(const char str[], int8_t index) {
	static char word[12];
	int8_t current_index=0;
	int8_t word_at=0;
	
	for(int8_t i=0; str[i] != '\0'; i++) {
		if(word_at == index) {	//Found target word, start copying into new array
			word[current_index] = str[i];
			current_index++;
		}
	
		if(str[i] == ' ') {	//Start of new word
			word_at++;
		}
	}
	
	word[current_index] = '\0'; //Null-terminate string
	return word;
}

