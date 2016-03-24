#define KEYBOARD_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/keyboard.c
//      Purpose:         Driver for the keyboard
//
//    Note: In the future, there should be a LOT less code in the
//          keyboard interrupt handler. It should only convert input
//          into a character, and place the character into a SMALL
//          character buffer, which can be accessed by the API
//
// ======================================================================== */

#include <keyboard.h>
#include <asmfunc.h>
#include <pic.h>
#include <idt.h>
#include <errors.h>
#include <vga.h>

volatile word key;
volatile bool keypress;
volatile bool listen;

static volatile bool shift;
static volatile bool caps;

/* Define the keyboard map */ 
struct __KEYMAP KEYMAP = {
   .lowercase = {
      '\0',
      ESC,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', /* 14 */
      '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* 28 */
      CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`',  LSHIFT, '\\', /* 43 */
            'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', /* 53 */
      RSHIFT, '*', ALT, ' ', CAPS, /* 58 */
      F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM, SCR, HOME, 
      UARROW, PGUP, '-', LARROW, '5', RARROW, '+', END, DARROW, PGDN, INS, DEL,
      F11, F12, LGUI, RGUI, APPS 
   },
   .uppercase = {
      '\0',
      ESC,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
      '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
      CTRL, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', LSHIFT, '|',
            'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 
      RSHIFT, '*', ALT, ' ', CAPS, 
      F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM, SCR, HOME, 
      UARROW, PGUP, '-', LARROW, '5', RARROW, '+', END, DARROW, PGDN, INS, DEL,
      F11, F12, LGUI, RGUI, APPS
   }
};


void kb_init(void)
{
   key = 0;
   shift = false;
   caps = false;
   listen = false;
   keypress = false;
   
   idt_add_handler((addr_t)keyboard_irq, IRQ_KEYBOARD);
   pic_enable_irq(IRQ_KEYBOARD);
   
   throw("Started keyboard driver",0);
}

void keyboard_handler(void)
{
   //print("Caught KB IRQ\n");
   byte status = inportb(KB_STATUS);
   word keycode;

   /* Lowest bit of status will be set if buffer is not empty */
   if (status & 0x01) {
      keycode = inportb(KB_DATA);
      
      /* Consider keycodes */
           if(keycode == LSHIFT_UP || keycode == RSHIFT_UP) { shift = false; }
      else if(keycode == LSHIFT_DN || keycode == RSHIFT_DN) { shift = true;  }
      else if(keycode == CAPS_DN) { caps = !caps; }
      else if(keycode < 127) {
         if(shift || caps) {
            keypress = true;
            key = KEYMAP.uppercase[keycode];
         }
         else {
            keypress = true;
            key = KEYMAP.lowercase[keycode];
         }
      }
   } /* if(status...) */
   pic_send_eoi(IRQ_KEYBOARD);
} /* extern void C_kb_driver(void) */
