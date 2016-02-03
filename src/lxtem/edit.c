/*
 *		Popcorn Kernel
 *		LXTEM Shell files
 *			edit.h -- Modify system variables
 */
 
void lxtem_edit(const char* args)
{
	char* operators = strword(args,2);
	for(int i=0; operators[i] != '\0'; i++) {
		switch(operators[i]) {
			case ':': break;
			default: break;
		}
	}
}