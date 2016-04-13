#define LEX_MAKE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/lex/lex-make.c                                   
//    Purpose:       Lex Command - Object creation                   
// ======================================================================== */

#include <lex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <anicafs.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

EXPORT_LEX("make", lex_make);
void lex_make(int argc, char* argv[])
{
   VALIDATE_ARG(argv[1], return);
   VALIDATE_ARG(argv[2], return);
   char* obj = argv[1];
   char* relpath = argv[2];
   
   char* path = lex_full_path(relpath);
  
   char* point = (char*) malloc(16);
   char* real_path = (char*) malloc(80);
   volume_t* vol = lex_read_mountpath(path, point, real_path);
   
   if(vol == NULL) {
      printf("make: Failed to read volume\n");
      goto exit;
   }
   
   /* Determine the object to be created */
   if(strcmp(obj, "dir") == 0) {
      /* TODO: Do not automatically assume floppy drive */
      if(!anica_mkdir(vol, real_path)) {
         printf("make: Could not create directory\n");
         goto exit;
      }
   }
   else if(strcmp(obj, "file") == 0) {
      anica_make_file(vol, real_path, 64);
   }
   else {
      printf("make: Unknown object `%s'\n", obj);
   }
   
   exit:
   free(point);
   free(real_path);
   free(path);
   return;
}
// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //