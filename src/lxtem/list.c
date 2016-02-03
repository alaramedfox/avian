/*
 *		Popcorn Kernel
 *		LXTEM Shell files
 *			list.h	-- lists information about various things
 */
 
void lxtem_list_help()
{
	print("List various objects.\n");
	print("   Usage: 'l :[options] [arg]'\n");
	print("Available options:\n");
	print("   :a - List all objects\n");
	print("   :d - List only directories\n");
	print("   :f - List only files\n");
	print("   :h - Include hidden objects\n");
	print("   :s - List objects that match search [arg]\n");
	
	print("   :! - Run as Captain\n");
	print("   :? - Show this help text\n");
	print("Note: As of Popcorn 1.2, most commands are not available.");
}	
 
void lxtem_list_all()
{
	print("Everything:\n");
	print(" -- Nothing here\n");
}

void lxtem_list_dir()
{
	print("Directories:\n");
	print(" -- No Directories\n");
}

void lxtem_list_std()
{
	print("Default list:\n");
	print(" -- Nothing here\n");
}
 
void lxtem_list_main(const char* options)
{
	for(int8_t i=0; options[i] != '\0'; i++) {
		switch(options[i])
		{
			case ':': /* nothing */ break;
			case 'a': lxtem_list_all(); break;
			case 'd': lxtem_list_dir(); break;
			case '?': lxtem_list_help(); break;
			default: break;
		}
	}
}
 
void lxtem_list(const char* args)
{
	print("\n");
	print("(");
	print(args);
	print(")");
	if(strwordcount(args) > 1) {
		/* Process arguments */
		char* arg1 = strword(args,1);
		if(arg1[0] == ':') {
			lxtem_list_main(arg1);
		}
		else {
			print("\nUnknown argument '");
			print(arg1);
			print("'\n");
		}
	} else {
		print("List requires (by default) at least 1 parameter.\n");
		print("You provided ");
		print(itos(strwordcount(args)-1));
		print(".\n");
	}
}