/*
 *		Popcorn Kernel
 *		LXTCM Shell files
 *			help.h -- Help documentation
 */
 
void lxtcm_help_basic()
{
	print("\n-- LXTCM shell version 0.1 --\n");
	print("  Basic commands:\n");
	print("  l - list items in current directory\n");
	print("  x - execute installed binary\n");
	print("  t - transfer object from a -> b\n");
	print("  c - copy an object from a -> b\n");
	print("  m - make an object here\n\n"); 
	print("To append options, use a colon (:) and list options.\n");
	print("   (e.g.) 'l :rh' to restrict listing to only hidden objects.\n");
}

void lxtcm_help_unknown(const char err[])
{
	print("\nLXTCM: Unreconized command: '");
	print(err);
	print("'.");
}