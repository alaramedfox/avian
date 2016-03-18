#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/include/errors.h                                    
//    Purpose:       Error flag implementation                    
// ======================================================================== */

#include <stdlib.h>
#include <string.h>

// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

void throw(const char[], int);   // Notify an error

#endif