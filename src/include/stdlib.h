#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
// ======================================================================== //
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/stdlib.h
//      Purpose:         Header for the C Standard Library
// ======================================================================== //
 
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* custom datatypes */
typedef uint16_t    index_t;
typedef uintptr_t addr_t;
typedef uint8_t   byte;      //8-bit value      1 byte
typedef uint16_t   word;      //16-bit value      2 bytes
typedef uint32_t   dword;    //32-bit value      4 bytes

/* Convert a symbol to a string */
#define QW(str)   #str

/* Bit manipulation macros */
#define bitmask(var,bit)   var & ~(1 << bit)
#define bitset(var,bit)      var | (1 << bit)
#define bitcheck(var,bit)   var & (1 << bit)

/* This creates a `foreach' style loop */
#define foreach(var, count)   for(int var=0; var<count; var++)
      
/* This compresses the packed attribute into something smaller */
#define FLAT   __attribute__((packed))

/* This enables code tracing, if a flag is set */
#if 1
   #define TRACE(str) print("("__FILE__ ":"); print(itoa(__LINE__,DEC)); \
                      print(") Says: `" str "'\n")
#else
   #define TRACE(str)
#endif

/* Prototypes for string-to-int functions */
double    atof(const char *str);
int       atoi(const char *str);
int       atol(const char *str);

double   strtod(const char *str, char **endptr);
int      strtol(const char *str, char **endptr, int base);

/*      
 *      MEMORY - Defined in mman.c
 */
#define  new(T) (T*)malloc(sizeof(T))
void*      calloc(size_t nitems, size_t size) __attribute__ ((malloc));
void      free(void *ptr);
void*      malloc(size_t size) __attribute__ ((malloc));
void*      realloc(void *ptr, size_t size);
size_t     ptrsize(void *ptr);

typedef struct __DIV
{
   int quot;
   int rem;
   
} div_t;

typedef struct __LDIV
{
   int quot;
   int rem;

} ldiv_t;

/*
 *      MATH - Defined in math.c
 */
div_t    div(int num, int den);
ldiv_t    ldiv(int num, int denom);

#endif