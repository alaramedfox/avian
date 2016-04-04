#define LEX_FS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          anica/lex/lex-fs.c                                    
//    Purpose:       Mount and manage filesystems                    
// ======================================================================== */

#include <lex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>
#include <filesystem.h>
#include <floppy.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

extern volatile byte floppy_cache_state;
extern volatile byte floppy_cache_value;

static void lex_fs_format(char* dev, char* fs);
static void lex_fs_cache(char* val);
static void lex_fs_help(void);

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

void lex_manf(int argc, char* argv[])
{
   if(argc >= 2 && argv[1][0] == ':') {
      int a = 2;
      char *dev, *fs;
      foreach(i, strlen(argv[1])) {
         switch(argv[1][i])
         {
            case ':': break;
            case '?': lex_fs_help(); break;
            case 'f':
               dev = ARGV(a);
               fs = ARGV(a);
               lex_fs_format(dev,fs); 
               break;
            case 'c': lex_fs_cache(ARGV(a)); break;
            
            default: break;
         }
      }
   }
   else {
      lex_fs_help();
   }
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void lex_fs_format(char* dev, char* fs)
{  
   if(dev == NULL || fs == NULL) {
      printf("Missing or invalid parameter\n");
      return;
   }
   device_t device = str_to_dev(dev);
   format_t filesystem = str_to_fs(fs);
   printf("Formatting %s as %s\n", dev, fs);
   format_device(device, filesystem);
}

static void lex_fs_help(void)
{
   printf("Usage: fs :[opt...] [val...]  - Manage a filesystem\n");
   printf(" ?                       Print this help text\n");
   printf(" f [device] [filesystem] Formats a device with the given filesystem\n");
   printf(" c [param]               Manage filesystem caching\n\n");
   
   printf("Use `fs :[opt] help' for details on a particular command\n");
}

static void lex_fs_cache(char* option)
{
   if(option == NULL) {
      printf("Missing or invalid parameter. See `fs :c help' for details\n");
      return;
   }  
   char** argv = (char**) malloc(2*sizeof(char**));
   int argc = split('=',0,option,argv);
   
   char* opt = argv[0];
   int val = argc==2?atoi(argv[1]):0;
   
   if(strcmp(opt,"disable") == 0) {
      floppy_cache_state = FDC_CACHE_DISABLE;
   }
   else if(strcmp(opt,"expand") == 0) {
      floppy_cache_state = FDC_CACHE_EXPAND;
      floppy_cache_value = val;
   }
   else if(strcmp(opt,"cap") == 0) {
      floppy_cache_state = FDC_CACHE_CAP;
      floppy_cache_value = val;
   }
   else if(strcmp(opt,"sync") == 0) {
      floppy_sync_cache();
   }
   else if(strcmp(opt,"clear") == 0) {
      floppy_clear_cache();
   }
   else if(strcmp(opt,"flush") == 0) {
      floppy_sync_cache();
      floppy_clear_cache();
   }
   else if(strcmp(opt,"status") == 0) {
      printf("Filesystem cache: ");
      switch(floppy_cache_state)
      {
         case FDC_CACHE_DISABLE: 
            printf("disabled\n"); break;
         case FDC_CACHE_EXPAND:
            printf("expands at %i entries\n",floppy_cache_value); break;
         case FDC_CACHE_CAP:
            printf("is capped at %i entries\n",floppy_cache_value); break;
         default:
            printf("is in an UNKNOWN state\n"); break;
      }
   }
   else if(strcmp(opt,"help") == 0) {
      printf("Usage: fs :c [option] - Manage filesystem caching\n");
      printf(" help       View this help text\n");
      printf(" disable    Disable caching completely\n");
      printf(" expand=N   Expand cache size by N units as needed\n");
      printf(" cap=N      Flush cache when N units are reached\n");
      printf(" sync       Write the cache to disk\n");
      printf(" clear      Clear the cache\n");
      printf(" flush      Sync, then clear\n");
      printf(" status     View the current cache options\n");
   }
   else {
      printf("Unknown option `%s'\n",argv[0]);
   }
   
   foreach(i, argc) {
      free(argv[i]);
   }
   free(argv);
}


