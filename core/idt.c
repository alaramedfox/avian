#define IDT_C_SOURCE
#include <idt.h>

/*
 *	Avian Project
 *	  File:		/core/idt.c
 *	  Purpose: 	Interrupt handler
 */
 
typedef struct __IDT_ENTRY
{
	uint16_t lower;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  type_attr;
	uint16_t higher;
	
} idt_entry_t;

static idt_entry_t idt_table[IDT_SIZE];

void idt_init(void)
{
	int32_t kaddr;
	int32_t iaddr;
	int32_t iptr[2];

	/* Populate IDT entry of keyboard's interrupt */
	kaddr = (int32_t)ASM_kb_driver;
	idt_table[ PIC1_COM ].lower 		= kaddr & 0xFFFF;
	idt_table[ PIC1_COM ].selector 	= KERNEL_CODE_SEGMENT_OFFSET;
	idt_table[ PIC1_COM ].zero			= 0;
	idt_table[ PIC1_COM ].type_attr 	= INTERRUPT_GATE;
	idt_table[ PIC1_COM ].higher 		= (kaddr & 0xFFFF0000) >> 16;

	/*
	 *	Port    	PIC1	PIC2
	 *	Command 	0x20	0xA0
	 *	Data	 	0x21	0xA1
	 */

	/* ICW1 - begin initialization */
	ASM_outb( PIC1_COM , 0x11);
	ASM_outb( PIC2_COM , 0x11);

	/* ICW2 - remap offset address of IDT
	 * In x86 protected mode, we have to remap the PICs beyond 0x20 because
	 * Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	 */
	ASM_outb( PIC1_DAT , 0x20);
	ASM_outb( PIC2_DAT , 0x28);

	/* ICW3 - setup cascading */
	ASM_outb( PIC1_DAT , 0x00);
	ASM_outb( PIC2_DAT , 0x00);

	/* ICW4 - environment info */
	ASM_outb( PIC1_DAT , 0x01);
	ASM_outb( PIC2_DAT , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	ASM_outb( PIC1_DAT , 0xff);
	ASM_outb( PIC2_DAT , 0xff);

	/* fill the IDT descriptor */
	iaddr = (int32_t)idt_table;
	iptr[0] = (sizeof (idt_entry_t) * IDT_SIZE) + ((iaddr & 0xffff) << 16);
	iptr[1] = iaddr >> 16 ;

	ASM_load_idt(iptr);
}
