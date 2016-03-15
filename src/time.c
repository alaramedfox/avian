#define TIME_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/time.c
//      Purpose:         Handle the PIT and time-related functions
// ======================================================================== */
 
#include <time.h>
#include <trace.h>
#include <pic.h>
#include <idt.h>
#include <asmfunc.h>
#include <vga.h>
#include <util.h>

static volatile uint32_t tick=0;

enum __PIT_PORTS
{
   PIT_DAT0 = 0x40,
   PIT_DAT1 = 0x41,
   PIT_DAT2 = 0x42,
   PIT_MODE = 0x43,
};

void time_init(word frequency)
{  function_call();

   /* Calculate the target frequency */
   word f = 1193180 / frequency;
   
   /* Send initialization command */
   outportb(PIT_MODE, 0x36);
   
   /* Split divisor into two bytes */
   byte low = (byte)(f & 0xFF);
   byte high = (byte)(f >> 8);
   
   /* Send the divisor data */
   outportb(PIT_DAT0, low);
   outportb(PIT_DAT0, high);
   
   idt_add_handler((addr_t)pit_irq, IRQ_PIT);
   pic_enable_irq(IRQ_PIT);
}

inline uint32_t clock(void)
{
   return tick;
}

void sleep(uint32_t ms)
{
   int t = tick;
   while(tick < t+ms);
}

__attribute__ ((hot)) void pit_handler(void) 
{
   tick++;
   pic_send_eoi(IRQ_PIT);
}




