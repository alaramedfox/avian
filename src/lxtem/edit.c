/*
 *		Popcorn Kernel
 *		LXTEM Shell files
 *			edit.h -- Modify system variables
 */
 
void lxtem_edit(string args)
{
	string operators = args.word(2);
	for(int i=0; operators[i] != '\0'; i++) {
		switch(operators[i]) {
			case ':': break;
			default: break;
		}
	}
}