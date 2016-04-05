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

#define generate_default_irq(num) throw_irq_##num

extern void throw_irq_0(void);
extern void throw_irq_1(void);
extern void throw_irq_2(void);
extern void throw_irq_3(void);
extern void throw_irq_4(void);
extern void throw_irq_5(void);
extern void throw_irq_6(void);
extern void throw_irq_7(void);
extern void throw_irq_8(void);
extern void throw_irq_9(void);
extern void throw_irq_10(void);
extern void throw_irq_11(void);
extern void throw_irq_12(void);
extern void throw_irq_13(void);
extern void throw_irq_14(void);
extern void throw_irq_15(void);
extern void throw_irq_16(void);
extern void throw_irq_17(void);
extern void throw_irq_18(void);


void exceptions_init(void)
{
   //foreach(i, 13) {
   //   idt_add_exception((addr_t)generate_default_irq(i), i);
   //}
   idt_add_exception((addr_t)generate_default_irq(0),0);
   idt_add_exception((addr_t)generate_default_irq(1),1);
   idt_add_exception((addr_t)generate_default_irq(2),2);
   idt_add_exception((addr_t)generate_default_irq(3),3);
   idt_add_exception((addr_t)generate_default_irq(4),4);
   idt_add_exception((addr_t)generate_default_irq(5),5);
   idt_add_exception((addr_t)generate_default_irq(6),6);
   idt_add_exception((addr_t)generate_default_irq(7),7);
   idt_add_exception((addr_t)generate_default_irq(8),8);
   idt_add_exception((addr_t)generate_default_irq(9),9);
   idt_add_exception((addr_t)generate_default_irq(10),10);
   idt_add_exception((addr_t)generate_default_irq(11),11);
   idt_add_exception((addr_t)generate_default_irq(12),12);
   idt_add_exception((addr_t)generate_default_irq(13),13);
   idt_add_exception((addr_t)generate_default_irq(14),14);
   idt_add_exception((addr_t)generate_default_irq(15),15);
   idt_add_exception((addr_t)generate_default_irq(16),16);
   idt_add_exception((addr_t)generate_default_irq(17),17);
   idt_add_exception((addr_t)generate_default_irq(18),18);
   
   idt_add_exception((addr_t)throw_zero_divide, 0);
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

void catch_zero_divide(dword eip)
{
   catch_exception(eip, 0);
}




