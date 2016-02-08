/*
 *		Popcorn Kernel
 *		LXTEM Shell files
 *			help.h -- Help documentation
 */
 
void lxtem_help_basic()
{
	stdout::print("\n-- LXTEM shell version 0.1 --\n\
			Basic commands:\n");
	stdout::print("  l - list items in current directory\n");
	stdout::print("  x - execute installed binary\n");
	stdout::print("  t - transfer/copy object from a -> b\n");
	stdout::print("  e - edit system variable\n");
	stdout::print("  m - make an object here\n"); 
	stdout::print("To append options, use a colon (:) and list options.\n");
	stdout::print("   (e.g.) use 'l :h' to list hidden objects.\n");
	stdout::print("Some options require an additional argument. These arguments\n");
	stdout::print("are entered after the operators, in the order operators appear.\n");
	stdout::print("   (e.g.) use 'e :bc 1 bw' to set blinking to 1 and colors to bw.\n");
}

void lxtem_help_unknown(string err)
{
	stdout::print("\nLXTEM: Unreconized command: '");
	stdout::print(err);
	stdout::print("'.");
}