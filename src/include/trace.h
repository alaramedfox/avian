#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/include/trace.h                                   
//    Purpose:       Provide function tracing utilities                    
// ======================================================================== */

/* Define the keyword 'this' to represent the function string */
#define this __func__

/* Macro for function tracing */
#define function_call()    record_function_call(__func__)
#define function_exit(x)   record_function_exit(__func__); return x

// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

void record_function_exit(const char[]);
void record_function_call(const char[]);
char* get_last_function(void);

#endif