#define EXCEPTIONS_C_SOURCE
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/exceptions.c
//      Purpose:        Exception handling
// ======================================================================== */
 
#include <exceptions.h>

#include <color.h>
#include <idt.h>
#include <vga.h>
#include <buildcount.h>
#include <util.h>
#include <stdlib.h>

#define FAULT(str)   print("\n\tError: "); print(str);
#define VALUE(val)   print("\n\tStack: "); iprint(val,16);

void exceptions_init(void)
{
   foreach(i, 13) {
      idt_add_exception((addr_t)throw_exception, i);
   }
   
   idt_add_exception((addr_t)throw_zero_divide, X_ZERO_DIVIDE);
   idt_add_exception((addr_t)throw_double_fault, X_DOUBLE_FAULT);
}

static void panic_screen(void)
{
   vga_setcolor(C_BLUESCR);   //Famous Windows bluescreen
   vga_clear();
   
   /* Print kernel information */
   print("\n\
   \tAVIAN Kernel - " TIMESTAMP "\n\n\
   \tAn unhandled exception has occured,\n\
   \tand execution was unable to continue.\n");
}

static void line_panic_screen(void)
{
   print("\n");
   int row = vga_getrow();
   vga_setcolor(C_BLUESCR);
   foreach(i, VGA_COL*2) {
      print(" ");
   }
   vga_movexy(row, 0);
   print("\tAVIAN Kernel - " TIMESTAMP);
}

void catch_exception(void)
{
   line_panic_screen();
   FAULT("Unknown exception");
   while(true);
}

void catch_double_fault(int arg)
{
   line_panic_screen();
   FAULT("Double Fault");
   VALUE(arg);
   while(true);
}

void catch_zero_divide(void)
{
   line_panic_screen();
   FAULT("Divide by zero");
   while(true);
}

