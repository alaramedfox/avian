#define MATH_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/math.c
//      Purpose:         Implementation of the Standard Library
// ======================================================================== */
 
#include <stdlib.h>

div_t div(int num, int den)
{
   div_t result;
   result.quot = num / den;
   result.rem = num % den;
   return result;
}

ldiv_t ldiv(int num, int den)
{
   ldiv_t result;
   result.quot = num / den;
   result.rem = num % den;
   return result;
}