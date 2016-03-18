#define EXCEPTIONS_C_SOURCE
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/exceptions.c
//      Purpose:        Exception handling
// ======================================================================== */
 
#include <exceptions.h>

#include <idt.h>
#include <stdio.h>
#include <vga.h>
#include <buildcount.h>
#include <util.h>
#include <stdlib.h>

void exceptions_init(void)
{
   foreach(i, 13) {
      idt_add_exception((addr_t)throw_exception, i);
   }
}

static void line_panic_screen(void)
{
   printf("\n");
   int row = vga_getrow();
   vga_setcolor(C_BLUESCR);
   foreach(i, VGA_COL*3) {
      printf(" ");
   }
   vga_movexy(row, 0);
   printf("\tAVIAN Kernel - " TIMESTAMP);
}

static char* get_error_id(byte err)
{
   switch(err)
   {
      case 0:  return "Divide by Zero";               break;
      case 1:  return "Debug";                        break;
      case 2:  return "Non-maskable interrupt";       break;
      case 3:  return "Breakpoint";                   break;
      case 4:  return "Overflow";                     break;
      case 5:  return "Bound Range Exceeded";         break;
      case 6:  return "Invalid Opcode";               break;
      case 7:  return "Device Not Available (IRQ7)";  break;
      case 8:  return "Double Fault";                 break;
      case 9:  return "Segment Overrun";              break;
      case 10: return "Invalid TSS";                  break;
      case 11: return "Segment Not Present";          break;
      case 12: return "Stack-Segment Fault";          break;
      case 13: return "General Protection Fault";     break;
      case 14: return "Page Fault";                   break;
      case 15: return "Reserved (IRQ15)";             break;
      case 16: return "Floating-Point Exception";     break;
      case 17: return "Alignment Check";              break;
      case 18: return "Machine Check";                break;
      case 19: return "SIMD Exception";               break;
      default: return "Invalid IRQ";                  break;
   }
}

void catch_exception(dword eip, byte exception)
{
   line_panic_screen();
   printf("\n\tError: %s",get_error_id(exception));
   printf("\n\tEIP: %x\t%x",eip,exception); 
   while(true);
}


