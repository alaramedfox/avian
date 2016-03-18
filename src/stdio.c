#define STDIO_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/stdio.c                                    
//    Purpose:       Standard input and output                    
// ======================================================================== */

#include <stdio.h>
#include <vga.h>
#include <string.h>
#include <envar.h>

extern volatile char key;

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

int scan(char* buffer, size_t len)
{
   ENVAR.FLAGS.listen = true;
   ENVAR.FLAGS.keypress = false;
   int vga_loc = vga_getloc();
   size_t loc=0;
   move_cursor(vga_getrow(), vga_getcol());
   
   while(key != '\n' && loc < len)
   {
      while(!ENVAR.FLAGS.keypress);
      
      if(key == '\b') {
         if(loc) {
            buffer[--loc] = 0;
         }
      }
      else {
         buffer[loc++] = key;
         buffer[loc] = 0;
      }
      
      vga_moveptr(vga_loc);
      print(buffer);
      move_cursor(vga_getrow(), vga_getcol());
      print(" ");
      
      ENVAR.FLAGS.keypress = false;
   }
   hide_cursor();
   ENVAR.FLAGS.listen = false;
   return loc;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //