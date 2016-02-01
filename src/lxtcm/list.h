/*
 *		Popcorn Kernel
 *		LXTCM Shell files
 *			list.h	-- lists information about various things
 */
 
void lxtcm_list_all()
{
	print("Everything:\n");
	print(" -- Nothing here\n");
}

void lxtcm_list_dir()
{
	print("Directories:\n");
	print(" -- No Directories\n");
}

void lxtcm_list_std()
{
	print("Default list:\n");
	print(" -- Nothing here\n");
}
 
void lxtcm_list_main(const char options[])
{
	for(int8_t i=0; options[i] != '\0'; i++) {
		switch(options[i])
		{
			case ':': /* nothing */ break;
			case 'a': lxtcm_list_all(); break;
			case 'd': lxtcm_list_dir(); break;
			default: lxtcm_list_std(); break;
		}
	}
}
 
void lxtcm_list(const char args[])
{
	print("\n");
	if(strwordcount(args) > 1) {
		/* Process arguments */
		char *arg1 = strword(args,1);
		if(arg1[0] == ':') {
			lxtcm_list_main(arg1);
		}
		else {
			print("\nUnknown argument '");
			print(arg1);
			print("'\n");
		}
	}
}