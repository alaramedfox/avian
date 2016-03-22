#define STRING_C_SOURCE
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/string.c
//      Purpose:        Implementation of the Standard Library string functions
//      (Most of this is based on the Linux implementation)
// ======================================================================== */
 
#include <string.h>

#include <stdlib.h>

/* Flag for which case to use for itoa */
volatile int itoa_case = UPPERCASE;

/* Flag for whether or not to use the '0x' hex prefix */
volatile bool itoa_hex_prefix = true;

static const char place_value[2][16] = { 
      "0123456789ABCDEF",
      "0123456789abcdef",
};

static const char bytes_sizes[2][6] = {
      "BKMGTP",
      "bkmgtp",
};

/**
 *    Avian_Documentation:
 *    Splits the given string @str according to the provided
 *    delimiter @delim. If the character given by @ignore appears,
 *    it is ignored. The resulting array of substrings is written
 *    to the provided array @array
 *    The function returns the number of elements split.
 */
size_t split(char delim, char ignore, const char str[], char** array)
{
   int pword = 0;
   int pletter = 0;
   array[0] = (char*) calloc(80,1);
   foreach(i, strlen(str)) {
      if(str[i] == delim) {
         array[pword++][pletter] = 0;
         pletter = 0;
         array[pword] = (char*) calloc(80,1);
      }
      else if(str[i] == ignore) {
         /* Do nothing */
      }
      else {
         array[pword][pletter++] = str[i];
      }
   }
   return pword+1;
}

void chomp(char str[])
{
   int i=0;
   while(str[i++] != '\n');
   str[i-1] = 0;
}

char* new_str(const char str[])
{  
   char* newstr = (char*) malloc(strlen(str));
   memcpy(newstr, str, strlen(str));
   return newstr;
}  

static inline void itoa_bytes(int number, char str[])
{  

   int magnitude = 0;
   
   while(number > 1024) {
      number = number/1024;
      magnitude++;
   }
   
   itoa(number,10, str);
   int len = strlen(str);
   str[len-2] = bytes_sizes[itoa_case][magnitude];
   str[len-1] = '\0';
}

void reverse(char s[])
{  //
   if(strlen(s) <= 1) return;
   int i, j;
   char c;

   for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
      c = s[i];
      s[i] = s[j];
      s[j] = c;
   }
}

void itoa(uint32_t number, base_t base, char str[])
{  
   if(base == BYTES) {
      itoa_bytes(number, str);
      return;
   }
   else if(base == BOOLEAN) {
      str = number?"true":"false";
      return;
   }
   else if(base == 0 || number == 0) {
      str[0] = '0';
      str[1] = '\0';
      return;
   }
   else {
      int pos=0, i;
      
      while(number > 0)
      {
         i = number % base;
         str[pos++] = place_value[itoa_case][i];
         number = number / base;
      }
      if(base == HEX && itoa_hex_prefix) {
         str[pos++] = 'x';
         str[pos++] = '0';
      }
      str[pos] = 0;
      if(pos) reverse(str);
   }
}

char *strcat(char *dest, const char *src)
{
   char *tmp = dest;

   while (*dest)
      dest++;
   while ((*dest++ = *src++) != '\0')
      ;
   return tmp;
}

char *strncat(char *dest, const char *src, size_t count)
{
   char *tmp = dest;

   if (count) {
      while (*dest)
         dest++;
      while ((*dest++ = *src++) != 0) {
         if (--count == 0) {
            *dest = '\0';
            break;
         }
      }
   }
   return tmp;
}

char *strchr(const char *s, int c)
{
   for (; *s != (char)c; ++s)
      if (*s == '\0')
         return NULL;
   return (char *)s;
}

int strcmp(const char *cs, const char *ct)
{
   unsigned char c1, c2;

   while (1) {
      c1 = *cs++;
      c2 = *ct++;
      if (c1 != c2)
         return c1 < c2 ? -1 : 1;
      if (!c1)
         break;
   }
   return 0;
}

int strncmp(const char *cs, const char *ct, size_t count)
{
   unsigned char c1, c2;

   while (count) {
      c1 = *cs++;
      c2 = *ct++;
      if (c1 != c2)
         return c1 < c2 ? -1 : 1;
      if (!c1)
         break;
      count--;
   }
   return 0;
}

char *strcpy(char *dest, const char *src)
{
   char *tmp = dest;

   while ((*dest++ = *src++) != '\0')
      /* nothing */;
   return tmp;
}

char *strncpy(char *dest, const char *src, size_t count)
{
   char *tmp = dest;

   while (count) {
      if ((*tmp = *src) != 0)
         src++;
      tmp++;
      count--;
   }
   return dest;
}

size_t strlen(const char* str)
{  //

   size_t size=0;
   while(str[size] != '\0') {
      ++size;
      if(size > 255) return size;
   }
   return size;
}

void *memset(void *s, int c, size_t count)
{
   char *xs = s;

   while (count--)
      *xs++ = c;
   return s;
}

int memcmp(const void *cs, const void *ct, size_t count)
{
   const unsigned char *su1, *su2;
   int res = 0;

   for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
      if ((res = *su1 - *su2) != 0)
         break;
   return res;
}




/* End string source code */
