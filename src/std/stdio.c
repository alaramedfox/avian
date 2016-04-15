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

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

typedef struct __SCAN_EVENTS
{
   word key;
   void (*event)(char*);
   bool ret;

} packed scan_event_t;

static scan_event_t event_list[16];
static size_t event_list_size=0;
static size_t event_list_max=16;

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

int getch(void)
{
   listen = true;
   keypress = false;
   while(!keypress);
   keypress = false;
   listen = false;
   return key;
}

/**
 *    Avian_Documentation:
 *    Registers a callback function for scan(), for use when
 *    a function has special behavior for a particular key.
 *    Registering an event requires the character, a pointer
 *    to the callback function, and a flag for whether or not
 *    the event causes scan() to return.
 *
 *    The event function takes one argument: a pointer to the
 *    scan input buffer, passed automatically by scan().
 *    The implementation can do whatever with
 *    it, or nothing at all.
 *
 *    The function returns 0 if the event was successfully added,
 *    and a nonzero value for any error (eg. no room in the list).
 *    NOTE: As of Avian 0.7.3, there is a maximum of 16 registered
 *    events.
 */
int register_scan_event(word key, void (*f)(char*), bool ret)
{
   if(event_list_size < event_list_max) {
      event_list[event_list_size].key = key;
      event_list[event_list_size].event = f;
      event_list[event_list_size].ret = ret;
      event_list_size++;
      return 0;
   }
   else return 1;
}

/**
 *    Avian_Documentation:
 *    Un-registers a registered key event. The function accepts
 *    the key that is used to trigger the event, and removes it
 *    from the list by swap removal.
 *
 *    The function returns 0 if the event was successfully removed,
 *    or nonzero if the event doesn't exist or there was some other
 *    error.
 */
int deregister_scan_event(word key)
{
   foreach(i, event_list_size) {
      if(event_list[i].key == key) {
         event_list[i] = event_list[event_list_size-1];
         event_list_size--;
         return 0;
      }
   }
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
   printf("%c",STRING_END);
   
   while(loc < BUFSIZE)
   {
      while(!keypress);
      
      foreach(i, event_list_size) {
         if(event_list[i].key == key) {
            event_list[i].event(buffer);
            loc = strlen(buffer);
            goto echo_buffer;
         }
      }
      
      if(key == '\b') {
         if(loc) {
            loc--;
            for(int i=loc; buffer[i] != 0; i++) {
               buffer[i] = buffer[i+1];
            }
            //buffer[loc] = 0;
         }
      }
      else if(key == '\0') {
         continue;
      }
      else if(key == '\n') {
         keypress = false;
         break;
      }
      else if(key == LARROW) {
         if(loc) loc--;
      }
      else if(key == RARROW) {
         if(loc<strlen(buffer)) loc++;
      }
      else {
         for(size_t i=strlen(buffer); i>loc; i--) {
            buffer[i] = buffer[i-1];
         }
         buffer[loc++] = key;
         //buffer[loc] = 0;
      }
      
      echo_buffer:
      vga_moveptr(vga_loc);
      printf("%s", buffer);
      move_cursor((loc+vga_loc)/VGA_COL, (loc+vga_loc)%VGA_COL);
      
      while(vga_getchar() != (char)STRING_END)
      {
         printf("%c",STRING_END);
      }
      
      
      keypress = false;
   }
   hide_cursor();
   listen = false;
   return loc;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //