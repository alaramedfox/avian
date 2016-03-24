#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/exceptions.h
//      Purpose:         Header for exception handling engine
// ======================================================================== */

#include <stdlib.h>
#include <string.h>

void exceptions_init(void);
extern void throw_exception(void);
extern void throw_zero_divide(void);

#endif