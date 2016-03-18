#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/include/stdarg.h                                    
//    Purpose:       Variadic argument lists                    
// ======================================================================== */

/* Note: This file requires GCC's builtin utilities */

#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)

typedef __builtin_va_list va_list;

#endif