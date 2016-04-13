#define LEX_ENTER_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                            
//    File:          avian/lex/lex-enter.c                                    
//    Purpose:       Lex enter commands                   
// ======================================================================== */

#include <lex.h>
#include <filesystem.h>
#include <anicafs.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

EXPORT_LEX("enter", "[path]", "Enter a subdirectory", lex_enter);
void lex_enter(int argc, char* argv[])
{
   VALIDATE_ARG(argv[1], return);
   char* relpath = argv[1];
   
   char* path = lex_full_path(relpath);
   size_t s1 = strlen(path);
   
   //if(anica_list_contents(path
  
   if(relpath[0] == ANICA_PARENT_DIR) {
      char** tree = (char**) malloc(64);
      size_t depth = split('/',0,path,tree);
      if(depth > 2) {
         size_t child_len = strlen(tree[depth-2]);
         path[s1-child_len-2] = 0;  // The -2 is for the "^"
         strcpy(current_directory, path);
      }
      foreach(i, depth) free(tree[i]);
      free(tree);
   }
   else if(relpath[0] == ANICA_CURRENT_DIR) {
      
   }
   else if(relpath[0] == ANICA_ROOT_DIR) {
      char** tree = (char**) malloc(64);
      size_t depth = split(':',0,path,tree);
      strcpy(current_directory, tree[0]);
      strcat(current_directory, ":$/");
      foreach(i, depth) free(tree[i]);
      free(tree);
   }
   else {
      if(path[s1-1] != '/') {
         path[s1] = '/';
         path[s1+1] = 0;
      }
      strcpy(current_directory, lex_full_path(path));
   }
   free(path);
}




