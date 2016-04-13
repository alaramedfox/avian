#define IDT_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb
//        File:            avian/idt.c
//        Purpose:       Interrupt Descriptor Table
// ======================================================================== */

#include <idt.h>
#include <asmfunc.h>

typedef struct __IDT_ENTRY
{
   word lower;
   word selector;
   byte zero;
   byte type_attr;
   word higher;
   
} packed idt_entry_t;

// ======================================================================== */
//       Static prototypes
// ======================================================================== */

static idt_entry_t idt_table[IDT_SIZE];

static void idt_add_interrupt(addr_t, byte);

// ======================================================================== */
//       Public API functions
// ======================================================================== */

void idt_write_table(void)
{
   dword descriptor[2] = { 
      (sizeof(idt_entry_t) * IDT_SIZE) + (((addr_t)idt_table & 0xffff) << 16),
      (addr_t)idt_table >> 16,
   };
   load_idt(descriptor);
}

void idt_add_handler(addr_t handler, byte irq)
{
   byte offset = 0x20 + irq;
   idt_add_interrupt(handler, offset);
}

void idt_add_exception(addr_t handler, byte err)
{
   byte offset = err;
   idt_add_interrupt(handler, offset);
}

// ======================================================================== */
//       Static functions
// ======================================================================== */

static void idt_add_interrupt(addr_t handler, byte offset)
{
   idt_table[offset].lower       = handler & 0xFFFF;
   idt_table[offset].selector    = KERNEL_OFFSET;
   idt_table[offset].zero          = 0;
   idt_table[offset].type_attr    = INTERRUPT_GATE;
   idt_table[offset].higher       = (handler & 0xFFFF0000) >> 16;
   
   //idt_write_table();
}






