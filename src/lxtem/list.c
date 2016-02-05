/*
 *		Popcorn Kernel
 *		LXTEM Shell files
 *			list.h	-- lists information about various things
 */
 
void lxtem_list_help()
{
	stdout::print("List various objects.\n");
	stdout::print("   Usage: 'l :[options] [arg]'\n");
	stdout::print("Available options:\n");
	stdout::print("   :a - List all objects\n");
	stdout::print("   :d - List only directories\n");
	stdout::print("   :f - List only files\n");
	stdout::print("   :h - Include hidden objects\n");
	stdout::print("   :s - List objects that match search [arg]\n");
	
	stdout::print("   :! - Run as Captain\n");
	stdout::print("   :? - Show this help text\n");
	stdout::print("Note: As of Popcorn 1.2, most commands are not available.");
}	
 
void lxtem_list_all()
{
	stdout::print("Everything:\n");
	stdout::print(" -- Nothing here\n");
}

void lxtem_list_dir()
{
	stdout::print("Directories:\n");
	stdout::print(" -- No Directories\n");
}

void lxtem_list_std()
{
	stdout::print("Default list:\n");
	stdout::print(" -- Nothing here\n");
}
 
void lxtem_list_main(string options)
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
 
void lxtem_list(string args)
{
	stdout::print("\n");
	stdout::print("(");
	stdout::print(args);
	stdout::print(")");
	if(args.wordcount() > 1) {
		/* Process arguments */
		string arg1 = args.getword(1);
		if(arg1[0] == ':') {
			lxtem_list_main(arg1);
		}
		else {
			stdout::print("\nUnknown argument '");
			stdout::print(arg1);
			stdout::print("'\n");
		}
	} else {
		stdout::print("List requires (by default) at least 1 parameter.\n");
		stdout::print("You provided ");
		stdout::print(itos(args.wordcount()-1));
		stdout::print(".\n");
	}
}