/*
*  link.ld
*/
OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

SECTIONS
 {
   . = 0x100000;
   .mbh   : { *(.mbh)   }
   .text  : { *(.text)  }
   .data  : { *(.data)  }
   .bss   : { *(.bss)   }
 }
 

