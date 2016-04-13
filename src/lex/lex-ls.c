#define FILE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/lex/lex-ld.c                                   
//    Purpose:       Directory listing LEX commands                    
// ======================================================================== */

#include <lex.h>
#include <stdlib.h>
#include <stdio.h>
#include <anicafs.h>
#include <vga.h>
#include <string.h>

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

/**
 *    Avian_Documentation:
 *    Lex Command - ls [path] :[options]
 *    Lists the contents of the current directory, or the specified
 *    path
 */
EXPORT_LEX("ls", "[path]", "List contents of path", lex_ls);
void lex_ls(int argc, char* argv[])
{
   int a = 1;
   char* relpath = ARGV(a);
   char* path = lex_full_path(relpath);
   
   if(path == NULL) {
      printf("Missing or invalid parameter\n");
      return;
   }
   
   char* point = (char*) malloc(16);
   char* real_path = (char*) malloc(80);
   volume_t* vol = lex_read_mountpath(path, point, real_path);

   if(vol == NULL) {
      printf("ls: Failed to read volume\n");
   }
   
   /* Read the actual list */
   char** list = (char**) malloc(64);
   
   int entries = anica_list_contents(vol, real_path, list);
   if(entries < 0) {
      switch(entries)
      {
         case ANICA_FSERR: printf("ls: %s is not a directory\n",path); break;
         case ANICA_NODIR: printf("ls: %s not found\n",path); break;
         case ANICA_IOERR: printf("ls: Input/Output error\n"); break;
         default: printf("ls: Unknown error\n"); break;
      }
      goto exit;
   }
   
   /* Print the directory contents */
   int tabsize = vga_tabsize(0);
   vga_tabsize(strlongest(list, entries));
   byte lfc = lex_file_color;
   byte ldc = lex_dir_color;
   byte lsc = lex_sys_color;
   byte ltc = lex_text_color;
   foreach(i, entries) {
      if(list[i] != NULL) {
         size_t len = strlen(list[i]);
         char* item = (char*) malloc(len+1);
         strcpy(item, list[i]);
         switch(item[len-1])
         {
            case ANICA_FILE_ICON:
               item[len-1] = 0;
               printf("%#%s%#%c\t", lfc, item, ltc, ANICA_FILE_ICON);
               break;
            case ANICA_DIR_ICON:
               item[len-1] = 0;
               printf("%#%s%#%c\t", ldc, item, ltc, ANICA_DIR_ICON);
               break;
            case ANICA_SYS_ICON:
               item[len-1] = 0;
               printf("%#%s%#%c\t", lsc, item, ltc, ANICA_SYS_ICON);
               break;
               
            default: printf("%s\t", item); break;
         }
      }
   }
   printf("\n");
   vga_tabsize(tabsize);
   
   exit:
   free(path);
   free(point);
   free(real_path);
   foreach(i, entries) free(list[i]);
   free(list);
   return;
}

