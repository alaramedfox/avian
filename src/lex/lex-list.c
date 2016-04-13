#define LEX_LIST_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/lex/lex-list.c                                    
//    Purpose:       Lex list functions                    
// ======================================================================== */

#include <lex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mman.h>
#include <util.h>
#include <vga.h>
#include <filesystem.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //


// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //


/**
 *    Avian_Documentation:
 *    Lex Command - memstat [options]
 *    Prints diagnostic memory information
 */
EXPORT_LEX("memstat", "[opt]", "Print memory information", lex_memstat);
void lex_memstat(int argc, char* argv[])
{
   bool detailed = false;
   
   printf("  Total     %h\t(%i bytes)\n",ALLOC_SIZE, ALLOC_SIZE);
   printf("  Used      %h\t(%i bytes)\n",mem_used(), mem_used());
   printf("  Free      %h\t(%i bytes)\n",mem_free(), mem_free());
   printf("  Blocks    %i\n\n",mem_blocks());
   
   if(detailed) {
      int tabs = vga_tabsize(0);
      vga_tabsize(10);
      foreach(i,79) printf("-");
      char format[] = "\r-%s\t%s\t%s\t%s\t%s\t%s\n";
      printf(format,"Index","Start","End","Offset","Size","Caller");
      foreach(i, mem_blocks()) {
         addr_t start;
         size_t size;
         char caller[FUNC_LEN];
         mem_block_info(i, &start, &size, caller);
         addr_t end = start+size;
         addr_t offset = start-ALLOC_START;
         printf("  %i:\t%x\t%x\t%x\t%i\t%s\n",i,start,end,offset,size,caller);
      }
      
      vga_tabsize(tabs);
   }
}



/**
 *    Avian_Documentation:
 *    Lex Command - varstat
 *    Prints diagnostic information about internal variables
 */
EXPORT_LEX("varstat", "-none-", "Print data size information", lex_varstat);
void lex_varstat(int argc, char* argv[])
{
   types_test();
}


/**
 *    Avian_Documentation:
 *    Lex Command - devstat
 *    Prints diagnostic information about mounted devices
 */
EXPORT_LEX("devstat", "-none-", "Print device information", lex_devstat);
void lex_devstat(int argc, char* argv[])
{
   printf("Scanning devices...\n");
   char* fs = read_fs(fda);
   printf("-Device--Format--Size---\n");
   if(fs != NULL) {
      printf(" (fda)   %s\t  1.44 MiB\n",fs);
   }
}



