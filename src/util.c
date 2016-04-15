#define UTIL_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/util.c
//      Purpose:         Provide misc functions
// ======================================================================== */

#include <util.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <vga.h>

#define psizeof(T)   printf("%A%s%A\t(%i)\t",F_GRN,#T,F_GRY,sizeof(T)*8)

void types_test(void)
{
   vga_tabsize(10);
   printf("- C Standard:\tC Library:\tFixed-width:\tCustom Avian:\n");
   printf("- "); psizeof(char);  psizeof(size_t);    psizeof(int8_t);  psizeof(byte); printf("\n");
   printf("- "); psizeof(short); psizeof(ptrdiff_t); psizeof(int16_t); psizeof(word); printf("\n");
   printf("- "); psizeof(int);   psizeof(intptr_t);  psizeof(int32_t); psizeof(dword); printf("\n");
   printf("- "); psizeof(long);  psizeof(intmax_t);  psizeof(int64_t); psizeof(qword); printf("\n");
   printf("- "); psizeof(bool); printf("\n");
   printf("\n");
   vga_tabsize(5);
}

void colors_test(void)
{
}

void trace_function(const char function[])
{
   printf("[ %A%s%A ]",F_MAG,function,F_GRY);
}

void notify_progress(const char str[], int current, int target)
{
   printf("%s (%i/%i)%c", str, current, target, current==target?'\n':'\r');
}








