#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/include/stdio.h
//      Purpose:        C Standard Library - Input / Output
// ======================================================================== */

#include <stdlib.h>
#include <stdarg.h>

#define BUFSIZE 80
#define STRING_END 255

typedef struct __FILE_STREAM
{
   char* name;
   void* data;
   
} FILE;

#define BOLD   "\x1b[1m"
#define TERM   "\x1b[0m"

#define F_BLK  "\x1b[30m"
#define F_BLU  "\x1b[31m"
#define F_GRN  "\x1b[32m"
#define F_CYN  "\x1b[33m"
#define F_RED  "\x1b[34m"
#define F_MAG  "\x1b[35m"
#define F_BRN  "\x1b[36m"
#define F_GRY  "\x1b[37m"

#define B_BLK  "\x1b[40m"
#define B_BLU  "\x1b[41m"
#define B_GRN  "\x1b[42m"
#define B_CYN  "\x1b[43m"
#define B_RED  "\x1b[44m"
#define B_MAG  "\x1b[45m"
#define B_BRN  "\x1b[46m"
#define B_GRY  "\x1b[47m"

enum __COLOR_DEFS
{
   C_TERM  = 0x07, C_BLUESCR=0x1F, 
   C_WARN  = 0x04, C_CRIT = 0x47,
};

typedef uint32_t fpos_t;
typedef uint32_t off_t;

int deregister_scan_event(word key);
int register_scan_event(word key, void (*f)(char*), bool ret);

int scan(char* buffer);

int printf(const char* format, ...);
int sprintf(char* str, const char* format, ...);
int vsprintf(char* str, const char* format, va_list args);
int scanf(const char* format, ...);


#endif