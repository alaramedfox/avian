/*
 *		Popcorn Kernel
 *			File:		string.h
 *			Purpose:	string-related functions
 */
 
bool string_compare(const char a[], const char b[])
{
   int c=0;

   while(a[c]==b[c])
   {
     if(a[c]=='\0'||b[c]=='\0')
     break;
     c++;
   }
   if(a[c]=='\0' && b[c]=='\0')
    	return 1;
   else
		return 0;
}

