#define STRING_C_SOURCE
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/string.c
//      Purpose:        Implementation of the Standard Library string functions
//      (Most of this is based on the Linux implementation)
// ======================================================================== */
 
#include <string.h>

#include <stdlib.h>

static const char place_value[] = "0123456789ABCDEF";
static const char bytes_magnitude[] = "BKMGTP";

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
   str[len-2] = bytes_magnitude[magnitude];
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
#define KR 0
#if KR
/* itoa:  convert n to characters in s */
 void itoa(uint32_t n, base_t base, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % base + '0';   /* get next digit */
     } while ((n /= base) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }
#else
void itoa(uint32_t number, base_t base, char str[])
{  //

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
         str[pos++] = place_value[i];
         number = number / base;
      }
      if(base == HEX) {
         str[pos++] = 'x';
         str[pos++] = '0';
      }
      str[pos] = 0;
      if(pos) reverse(str);
   }
}
#endif
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
