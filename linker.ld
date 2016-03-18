/*
*  link.ld
*/
OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
/*
MEMORY
 {
 	kern (wx) : ORIGIN = 0x100000, LENGTH = 64K
 	vram (wa) : ORIGIN = 0x0b8000, LENGTH = 4096
 }
*/
SECTIONS
 {
   . = 0x7E00;
   .mbh  : { *(.mbh)  }
   .text : { *(.text) }
   .data : { *(.data) }
   .bss  : { *(.bss)  }
 }
 

