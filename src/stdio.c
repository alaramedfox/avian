#define STDIO_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/stdio.c                                    
//    Purpose:       Standard input and output                    
// ======================================================================== */

#include <stdio.h>
#include <stdarg.h>
#include <vga.h>
#include <string.h>
#include <keyboard.h>

extern volatile int itoa_case;
extern volatile bool itoa_long;

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

static void print(const char str[])
{
   for(size_t i=0; str[i] != '\0'; i++) {
      addch(str[i]);
   }
}

static void iprint(int num, int base, int chcase, bool prefix)
{
   itoa_case = chcase;
   itoa_long = prefix;
   
   char* str = (char*) malloc(11);
   itoa(num, base, str);
   print(str);
   free(str);
}

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

int printf(const char* format, ...)
{
   va_list args;
   va_start(args, format);
   bool escape = false;
   bool long_escape = false;
   
   foreach(i, strlen(format)) {
      if(escape) {
         switch(format[i])
         {
            case '{': long_escape = true;                break;
            case '}': long_escape = false;               break;
            case '#': vga_setcolor(va_arg(args,int));    break;
            case 'c': addch(va_arg(args,int));           break;
            case 'd':
            case 'i': iprint(va_arg(args,int),DEC,0,0);  break;
            case 's': print(va_arg(args,char*));         break;
            case 'x': iprint(va_arg(args,int),HEX,1,1);  break;
            case 'X': iprint(va_arg(args,int),HEX,0,0);  break;
            case '%': addch('%');                        break;
            case 'h': iprint(va_arg(args,int),BYTES,0,1);break;
            case 'H': iprint(va_arg(args,int),BYTES,1,1);break;
            default: break;
         }
         escape = false || long_escape;
      }
      else if(format[i] == '%') {
         escape = true;
      }
      else {
         addch(format[i]);
      }
   }
   va_end(args);
   return 1;
}

int scan(char* buffer)
{
   key = 0;
   listen = true;
   keypress = false;
   int vga_loc = vga_getloc();
   size_t loc=0;
   move_cursor(vga_getrow(), vga_getcol());
   //buffer = (char*) calloc(BUFSIZE, 1);
   
   while(loc < BUFSIZE)
   {
      while(!keypress);
      
      if(key == '\b') {
         if(loc) {
            buffer[--loc] = 0;
         }
      }
      else if(key == '\0') {
         continue;
      }
      else if(key == '\n') {
         keypress = false;
         break;
      }
      else {
         buffer[loc++] = key;
         buffer[loc] = 0;
      }
      
      vga_moveptr(vga_loc);
      print(buffer);
      move_cursor(vga_getrow(), vga_getcol());
      print(" ");
      
      keypress = false;
   }
   hide_cursor();
   listen = false;
   return loc;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //