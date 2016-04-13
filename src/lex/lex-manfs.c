#define LEX_FS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          anica/lex/lex-manfs.c                                    
//    Purpose:       Mount and manage filesystems                    
// ======================================================================== */

#include <lex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>
#include <filesystem.h>
#include <floppy.h>
#include <vga.h>



// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //



extern volatile byte floppy_cache_state;
extern volatile byte floppy_cache_value;


// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

EXPORT_LEX("format", lex_format);
void lex_format(int argc, char* argv[])
{  
   VALIDATE_ARG(argv[1], return);
   VALIDATE_ARG(argv[2], return);
   char* point = argv[1];
   char* fs = argv[2];
   
   device_t device;
   
   foreach(i, mounted_volumes) {
      if(strcmp(point, mountpoints[i].point) == 0) {
         device_t device = mountpoints[i].device;
         format_t filesystem = str_to_fs(fs);
         printf("Formatting %s as %s\n", point, fs);
         format_device(device, filesystem);
         return;
      }
   }
   
   printf("You need to mount a device before formatting\n");
}





/**
 *    Avian_Documentation:
 *    Lex Command - devlist
 *    Lists all currently mounted devices and their mountpoints
 */
EXPORT_LEX("devlist", lex_devlist);
void lex_devlist(int argc, char* argv[])
{
   foreach(i, 79) printf("-");
   printf("\r\tDevice\tMount Point\tLabel\n");
   foreach(i, mounted_volumes) {
      device_t dev = mountpoints[i].device;
      char* point = mountpoints[i].point;
      char* label = mountpoints[i].vol->sb.label;
      printf("\t%s\t\t%s\t%s\n",dev_to_str(dev),point,label);
   }
   printf("\n");
}


/**
 *    Avian_Documentation:
 *    Lex Command - mount [device] [mountpoint]
 *    Mounts the specified device using the mountpoint as a label
 */
EXPORT_LEX("mount", lex_mount);
void lex_mount(int argc, char* argv[])
{
   VALIDATE_ARG(argv[1], return);
   VALIDATE_ARG(argv[2], return);
   char* dev = argv[1];
   char* point = argv[2];
   
   if(mounted_volumes == MAX_LEX_MOUNTS) {
      printf("Maximum number of volumes have been mounted\n");
      return;
   }
   
   device_t device = str_to_dev(dev);
   
   foreach(i, mounted_volumes) {
      if(strcmp(mountpoints[i].point, point) == 0) {
         printf("%s is already a mount point\n",point);
         return;
      }
      if(mountpoints[i].device == device) {
         printf("%s is already mounted as `%s'\n",dev,
            mountpoints[i].point);
         return;
      }
   }
   
   if(device >= NO_DEV) {
      printf("Device `%s' does not exist\n",dev);
      return;
   }
   
   mountpoints[mounted_volumes].device = device;
   mountpoints[mounted_volumes].point = (char*) malloc(strlen(point));
   strcpy(mountpoints[mounted_volumes].point, point);
   mountpoints[mounted_volumes].vol = mount(device);
   mounted_volumes++;
}


/**
 *    Avian_Documentation:
 *    Lex Command - umount [mountpoint]
 *    Unmounts the specified mount point
 */
EXPORT_LEX("umount", lex_umount);
void lex_umount(int argc, char* argv[])
{
   VALIDATE_ARG(argv[1], return);
   char* point = argv[1];
   
   foreach(i, mounted_volumes) {
      if(strcmp(mountpoints[i].point, point) == 0) {
         unmount(mountpoints[i].vol);
         
         mountpoints[i] = mountpoints[--mounted_volumes];
         return;
      }
   }
   
   printf("`%s' does not appear to be an active mount point\n",point);
}

/**
 *    Avian_Documentation:
 *    Lex Command - fsdump [format] [sector]
 *    Prints raw data to screen from the given sector on disk
 */
EXPORT_LEX("fsdump", lex_fsdump);
void lex_fsdump(int argc, char* argv[])
{
   VALIDATE_ARG(argv[1], return);
   VALIDATE_ARG(argv[2], return);
   char* fmt = argv[1];
   char* sec = argv[2];
   
   int format=0;
   if(strcmp(fmt,"hex")==0) format = 1;
   else if(strcmp(fmt,"char")==0) format = 2;
   else printf("Invalid dump format\n");
   
   int sector = atoi(sec);
   byte* data = (byte*) malloc(512);
   floppy_read_block(sector, data, 512);
   
   foreach(i, 512) {
      if(format == 1) {
         if(i && i%4==0) printf(" ");
         if(i%32==0) printf("\n%#+%i%#\t",GREEN,i,C_TERM);
         printf(data[i]<=0xf?"0%X":"%X",data[i]);
      }
      else if(format == 2) {
         if(i%64==0) printf("\n%#+%i%#\t",GREEN,i,C_TERM);
         printf("%c",data[i]<32?249:data[i]);
      }
   }
   printf("\n\n");
   free(data);
}

EXPORT_LEX("fscache", lex_fscache);
void lex_fscache(int argc, char* argv[])
{
   int a = 1;
   char* opt = ARGV(a);
   VALIDATE_ARG(opt, return);
   
   int val = argc==3?atoi(ARGV(a)):0;
   
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


