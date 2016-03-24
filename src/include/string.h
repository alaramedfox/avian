#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/string.h
//      Purpose:         C Standard Library String utilities
// ======================================================================== */

#include <stddef.h>

typedef enum __BASE
{
   DEC=10, HEX=16, OCT=8, BIN=2, BYTES=254, BOOLEAN=255,
   
} base_t;

enum __ITOA_CASES
{
   UPPERCASE = 0, LOWERCASE = 1,
};

size_t split(char, char, const char[], char**);
void   chomp(char[]);
char*  new_str(const char[]);

void*  memchr(const void *str, int c, size_t n);
int    memcmp(const void *str1, const void *str2, size_t n);
void*  memcpy(void *dest, const void *src, size_t n);
void*  memmove(void *dest, const void *src, size_t n);
void*  memset(void *str, int c, size_t n);

char*  strcat(char *dest, const char *src);
char*  strncat(char *dest, const char *src, size_t n);
char*  strchr(const char *str, int c);
int    strcmp(const char *str1, const char *str2);
int    strncmp(const char *str1, const char *str2, size_t n);
char*  strcpy(char *dest, const char *src);
char*  strncpy(char *dest, const char *src, size_t n);

void   itoa(unsigned int number, base_t base, char str[]);
size_t strlen(const char *str);





#endif