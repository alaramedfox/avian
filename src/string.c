#define STRING_C_SOURCE
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/string.c
//      Purpose:        Implementation of the Standard Library string functions
//      (Most of this is based on the Linux implementation)
// ======================================================================== */
 
#include <string.h>
#include <stdlib.h>

/**
 *    Avian_Documentation: 
 *    Flags that determine which letter case itoa() uses,
 *    as well as whether or not to use the long notation
 *    for various number bases. In hex, the long notation
 *    adds a '0x' -- for human-readable data sizes, it adds
 *    the xiB to the end (like KiB or MiB)
 *
 *    If uppercase, itoa_bytes() uses base-10 byte magnitudes
 *    (1 MB = 1000 B). Otherwise, itoa_bytes() uses base-2
 *    magnitudes (1 MiB = 1024 B).
 */ 
volatile int itoa_case = UPPERCASE;
volatile bool itoa_long = true;

static const char place_value[2][16] = { 
      "0123456789ABCDEF",
      "0123456789abcdef",
};

static const char bytes_sizes[6] = "BKMGTP";

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
      if(str[i] == delim && pletter == 0) {
         /* Do nothing */
      }
      else if(str[i] == delim) {
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
   foreach(i, strlen(str)) {
      if(str[i] == '\n') {
         str[i] = 0;
         break;
      }
   }
}

char* new_str(const char str[])
{  
   char* newstr = (char*) malloc(strlen(str)+1);
   //memcpy(newstr, str, strlen(str));
   strcpy(newstr, str);
   return newstr;
}

size_t strlongest(char** strings, int num)
{
   size_t record = 0;
   foreach(i, num) {
      size_t len = strlen(strings[i]);
      if(len > record) record = len;
   }
   return record;
}

/**
 *    Avian_Documentation:
 *    Merges an array of strings into the given string,
 *    then returns a pointer to that string.
 *    If a delimeter is provided (eg, not NULL), then that 
 *    delimeter is used to separate the concatenations.
 */
char* strmerge(char* dest, char** src, int num, const char* delim)
{
   foreach(i, num) {
      if(delim != NULL) strcat(dest, delim);
      strcat(dest, src[i]); 
   }
   
   return dest;
}

static inline void itoa_bytes(int number, char str[])
{  
   int magnitude = 0;
   int divisor = itoa_case?1000:1024;
   int rem = number % divisor;
   
   while(number > divisor) {
      number = number/divisor;
      magnitude++;
   }
   
   itoa(number,10, str);
   int len = strlen(str);
   
   if(magnitude && rem > 100 && rem < (divisor-100)) {
      char fraction[5];
      itoa(rem, DEC, fraction);
      str[len++] = '.';
      str[len++] = fraction[0];
   }
   
   if(itoa_long) str[len++] = ' ';
   
   str[len++] = bytes_sizes[magnitude];
   
   if(itoa_long && magnitude) {
      if(!itoa_case) { str[len++] = 'i'; }
      str[len++] = 'B';
      
   }
   str[len] = '\0';
}

void reverse(char s[])
{
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
      if(base == HEX && itoa_long) {
         str[pos++] = 'x';
         str[pos++] = '0';
      }
      str[pos] = 0;
      if(pos) reverse(str);
   }
}

static char* make_numeric(const char* str)
{
   char* numeric = (char*) malloc(16);
   int pos=0;
   foreach(i, strlen(str)) {
      if(str[i] >= '0' && str[i] <= '9') {
         numeric[pos++] = str[i];
      }
   }
   numeric[pos] = 0;
   return numeric;
}

int atoi(const char* str)
{
   int num = 0;
   char* numeric = make_numeric(str);
   int len = strlen(numeric);
  
   foreach(i, len) {
      num += (numeric[i]-'0') * pow(10, len-i-1);
   }
   
   free(numeric);
   return num;
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

void* memcpy(void *str1, const void *str2, size_t n)
{
   byte *source = (byte*)str2;
   byte *dest = (byte*)str1;
   
   foreach(i, n) {
      dest[i] = source[i];
   }
   return dest;
}


/* End string source code */
