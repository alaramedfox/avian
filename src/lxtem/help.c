/*
 *		Popcorn Kernel
 *		LXTEM Shell files
 *			help.h -- Help documentation
 */
 
void lxtem_help_basic()
{
	print("\n-- LXTEM shell version 0.1 --\n\
			Basic commands:\n");
	print("  l - list items in current directory\n");
	print("  x - execute installed binary\n");
	print("  t - transfer/copy object from a -> b\n");
	print("  e - edit system variable\n");
	print("  m - make an object here\n"); 
	print("To append options, use a colon (:) and list options.\n");
	print("   (e.g.) use 'l :h' to list hidden objects.\n");
	print("Some options require an additional argument. These arguments\n");
	print("are entered after the operators, in the order operators appear.\n");
	print("   (e.g.) use 'e :bc 1 bw' to set blinking to 1 and colors to bw.\n");
}

void lxtem_help_unknown(const string err)
{
	print("\nLXTEM: Unreconized command: '");
	print(err);
	print("'.");
}