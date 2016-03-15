#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/exceptions.h
//      Purpose:         Header for exception handling engine
// ======================================================================== */

#include <stdlib.h>
#include <string.h>



typedef enum __CPU_EXCEPTIONS
{
   X_ZERO_DIVIDE=0, X_DOUBLE_FAULT=8, X_RANGE_BOUND=5, 
   X_GEN_FAULT=13, X_PAGE_FAULT=14, X_STACK_FAULT=12,

} exception_t;

void exceptions_init(void);
extern void throw_exception(void);

/* -- Exception handlers --    |  -- Exceptions thrown via ASM -- */

void catch_zero_divide   (void); 
void catch_debug         (void);
void catch_nmi            (void);
void catch_breakpoint   (void);
void catch_overflow      (void);
void catch_range_bound   (void);
void catch_bad_opcode   (void);
void catch_no_device      (void);
void catch_double_fault   (int);   extern void throw_double_fault(int);
void catch_bad_tss      (int);
void catch_no_segment   (int);
void catch_stack_fault   (int);
void catch_gen_fault      (int);
void catch_page_fault   (int);
void catch_float_err      (void);
void catch_align_check   (int);
void catch_machine_check(void);
void catch_simd_err      (void);
void catch_virt_err      (void);
void catch_triple_fault   (void);

#endif