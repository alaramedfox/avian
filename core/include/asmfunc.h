#ifndef ASM_FUNC_H_INCLUDED
#define ASM_FUNC_H_INCLUDED
/*
 *		Avian Project - Bryan Webb
 *		File:		/core/include/asmfunc.h
 *		Purpose:	Header for all global assembly functions
 */
 
#include <types.h>

/*
enum __FLP_PORTS
{
	STATUS_A	= 0x3F0, // read-only
	STATUS_B	= 0x3F1, // read-only
	DOR      = 0x3F2,
	TDR    	= 0x3F3,
	MSR		= 0x3F4, // read-only
	DSR      = 0x3F4, // write-only
	FIFO     = 0x3F5,
	DIN      = 0x3F7, // read-only
	CCR      = 0x3F7  // write-only
};
*/

enum __FLOPPY_PORTS
{
	FDC_STA=0x3F0, FDC_STB=0x3F1, FDC_DOR=0x3F2, FDC_TDR=0x3F3,
	FDC_MSR=0x3F4, FDC_DSR=0x3F4, FDC_FIFO=0x3F5, FDC_DIN=0x3F7,
	FDC_CCR=0x3F7 
};

enum __PIC_PORTS
{
	PIC1_COM=0x20, PIC1_DAT=0x21,
	PIC2_COM=0xA0, PIC2_DAT=0xA1
};

extern byte ASM_inb(port_t);
extern void ASM_outb(port_t, byte);
extern void ASM_load_idt(int32_t*);
extern void ASM_kb_driver(void);
extern void ASM_shut_down(void);


#endif