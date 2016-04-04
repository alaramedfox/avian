#define LEX_FS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          anica/lex/lex-fs.c                                    
//    Purpose:       Mount and manage filesystems                    
// ======================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>
#include <filesystem.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

extern volatile bool floppy_use_cache;

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
      foreach(i, strlen(argv[1])) {
         switch(argv[1][i])
         {
            case ':': break;
            case '?': lex_fs_help(); break;
            case 'f': lex_fs_format(argv[a], argv[a+1]); a+= 2; break;
            case 'c': lex_fs_cache(argv[a++]); break;
            
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
   device_t device = str_to_dev(dev);
   format_t filesystem = str_to_fs(fs);
   printf("Formatting %s as %s\n", dev, fs);
   format_device(device, filesystem);
}

static void lex_fs_help(void)
{
   printf("Usage: m :[opt...] [val...]  - Manage a filesystem\n");
   printf(" ?                       Print this help text\n");
   printf(" f [device] [filesystem] Formats a device with the given filesystem\n");
   printf(" c [value]               Enable/Disable filesystem caching (0/1)\n");
}

static void lex_fs_cache(char* val)
{
   floppy_use_cache = atoi(val);
}


