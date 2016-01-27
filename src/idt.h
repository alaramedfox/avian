/*
 *	Popcorn Kernel
 *	  File:		idt.h
 *	  Purpose: 	IRQ, etc
 */
 
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

extern char read_port(int16_t port);
extern void write_port(int16_t port, int8_t data);
extern void load_idt(int32_t *iptr);

struct IDT_entry {
	int16_t offset_lowerbits;
	int16_t selector;
	int8_t zero;
	int8_t type_attr;
	int16_t offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];


void idt_init(void)
{
	int32_t kaddr;
	int32_t iaddr;
	int32_t iptr[2];

	/* populate IDT entry of keyboard's interrupt */
	kaddr = (int32_t)keyboard_handler;
	IDT[0x21].offset_lowerbits = kaddr & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (kaddr & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	iaddr = (int32_t)IDT ;
	iptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((iaddr & 0xffff) << 16);
	iptr[1] = iaddr >> 16 ;

	load_idt(iptr);
}
