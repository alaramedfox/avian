#define PIC_C_SOURCE
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/pic.c
//      Purpose:        Programmable Interrupt Controller
// ======================================================================== */
 
#include <pic.h>
#include <asmfunc.h>

static void pic_send_command(pic_cmd_t);
static void pic_send_data(byte, byte);

void pic_init(void)
{
   pic_send_command(PIC_INIT);         // ICW1 - Begin init
   pic_send_data(OFFSET1, OFFSET2);    // ICW2 - Remap IDT
   pic_send_data(0x00, 0x00);          // ICW3 - Setup cascading
   pic_send_data(0x01, 0x01);          // ICW4 - Enviornment info
   
   pic_send_data(0xFF, 0xFF);
}

void pic_send_eoi(interrupt_t irq)
{
   /* If using PIC2, EOI must be sent to both PICs */
   if(irq >= 8) { outportb( PIC2_CMD, PIC_EOI ); }
   outportb( PIC1_CMD, PIC_EOI );
}

void pic_enable_irq(interrupt_t irq)
{
   uint16_t port;
   uint8_t value;
 
   if(irq < 8) {
      port = PIC1_DAT;
   } else {
      port = PIC2_DAT;
      irq -= 8;
   }
   value = inportb(port) & ~(1 << irq);
   outportb(port, value);   
}

void pic_disable_irq(interrupt_t irq)
{
   uint16_t port;
   uint8_t value;
 
   if(irq < 8) {
      port = PIC1_DAT;
   } else {
      port = PIC2_DAT;
      irq -= 8;
   }
   value = inportb(port) | (1 << irq);
   outportb(port, value);
}

static inline void pic_send_command(pic_cmd_t cmd)
{   
   outportb(PIC1_CMD, cmd);
   outportb(PIC2_CMD, cmd);
}

static inline void pic_send_data(byte data1, byte data2)
{
   outportb(PIC1_DAT, data1);
   outportb(PIC2_DAT, data2);
}

void pic_remap(void)
{
   /* Save current IRQ masks */
   byte mask1 = inportb(PIC1_DAT);
   byte mask2 = inportb(PIC2_DAT);
   
   pic_init();
   
   pic_send_data(mask1, mask2);       // Restore saved IRQ masks
}





