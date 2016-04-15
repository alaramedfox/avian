#define PRINTF_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/std/printf.c                                    
//    Purpose:       C Standard Library print functions                    
// ======================================================================== */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vga.h>

static int iapp(char* str, int num, int base);
static int sapp(char* str, const char* src);
static int capp(char* str, int ch);
static void apply_ansi_code(int code);
static void print(const char[]);

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

int vsprintf(char* str, const char* format, va_list args)
{
   bool escape = false;
   
   foreach(i, strlen(format)) {
      if(escape) {
         switch(format[i])
         {
            case '#': va_arg(args, int);                 break;
            case 'A': sapp(str, va_arg(args,char*));     break;
            case 'c': capp(str, va_arg(args,int));       break;
            case 'd':
            case 'i': iapp(str, va_arg(args,int),DEC);   break;
            case 's': sapp(str, va_arg(args,char*));     break;
            case 'x': iapp(str, va_arg(args,int),HEX);   break;
            case 'X': iapp(str, va_arg(args,int),HEX);   break;
            case '%': capp(str, '%');                    break;
            case 'h': iapp(str, va_arg(args,int),BYTES); break;
            case 'H': iapp(str, va_arg(args,int),BYTES); break;
            default: break;
         }
         escape = false;
      }
      else if(format[i] == '%') {
         escape = true;
      }
      else {
         capp(str, format[i]);
      }
   }
   va_end(args);
   return strlen(str);
}

int sprintf(char* str, const char* format, ...)
{
   va_list args;
   va_start(args, format);
   
   return vsprintf(str, format, args);
}

int printf(const char* format, ...)
{
   va_list args; 
   va_start(args, format);
   char* str = (char*) malloc(1024);
   int len = vsprintf(str, format, args);
   print(str);
   return len;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void apply_ansi_code(int code)
{
   switch(code)
   {
      case 0: vga_setcolor(0x07); break;
      case 1: vga_setcolor(vga_getcolor() | 8); break;
      case 7: vga_setcolor(~vga_getcolor()); break;  
      default: break;
   }
   /* Set foreground color */
   if(code >= 30 && code <= 37) {
      byte color = vga_getcolor();
      byte high = color & 0xF0;
      vga_setcolor(high | (code-30));
   }
   else if(code >= 40 && code <= 47) {
      byte color = vga_getcolor();
      byte low = color & 0x0F;
      vga_setcolor(((code-40) << 4) | low);
   }
}

static void print(const char str[])
{
   if(str == NULL) {
      print("NULL");
      return;
   }
   
   for(size_t i=0; str[i] != '\0'; i++) {
      if(str[i] == '\x1b' && str[i+1] == '[') {
         char* code = (char*) malloc(4);
         for(; str[i] != 'm' && str[i] != '\0'; i++) {
            capp(code, str[i]);
         }
         apply_ansi_code(atoi(code));
      }
      else {
         addch(str[i]);
      }
   }
}

static int iapp(char* str, int num, int base)
{
   char* new_str = (char*) malloc(11);
   itoa(num, base, new_str);
   strcat(str, new_str);
   free(new_str);
   return strlen(str);
}

static int sapp(char* str, const char* src)
{
   strcat(str, src);
   return strlen(str);
}

static int capp(char* str, int ch)
{
   int len = strlen(str);
   str[len] = (char)ch;
   str[len+1] = 0;
   return strlen(str);
}


