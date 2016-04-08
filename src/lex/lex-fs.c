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
#include <vga.h>

#define MAX_LEX_MOUNTS 8

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

typedef struct __LEX_FS_MOUNTPOINT
{
   device_t device;
   char* point;
   volume_t* vol;
   
} packed lex_fs_mpoint_t;

static lex_fs_mpoint_t mountpoints[MAX_LEX_MOUNTS];
static size_t mounted_volumes=0;

extern volatile byte floppy_cache_state;
extern volatile byte floppy_cache_value;

static void lex_fs_format(char* dev, char* fs);
static void lex_fs_dump(char* dev, char* sec);
static void lex_fs_cache(char* val);
static void lex_fs_help(void);
static void lex_fs_mount(char* dev, char* point);
static void lex_fs_unmount(char* point);
static void lex_fs_list(void);
static void lex_fs_new(char* obj, char* path);
static void lex_fs_dirlist(char* path);

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

void lex_manf(int argc, char* argv[])
{
   if(argc >= 2 && argv[1][0] == ':') {
      int a = 2;
      char *arg1, *arg2;
      foreach(i, strlen(argv[1])) {
         switch(argv[1][i])
         {
            case ':': break;
            case '?': lex_fs_help(); break;
            case 'f':
               arg1 = ARGV(a);
               arg2 = ARGV(a);
               lex_fs_format(arg1,arg2); 
               break;
            case 'c': lex_fs_cache(ARGV(a)); break;
            case 'd':
               arg1 = ARGV(a);
               arg2 = ARGV(a);
               lex_fs_dump(arg1,arg2);
               break;
            case 'm':
               arg1 = ARGV(a);
               arg2 = ARGV(a);
               lex_fs_mount(arg1,arg2);
               break;
            case 'n':
               arg1 = ARGV(a);
               arg2 = ARGV(a);
               lex_fs_new(arg1,arg2);
               break;
               
            case 'u': lex_fs_unmount(ARGV(a)); break;
            case 'l': lex_fs_dirlist(ARGV(a)); break;
            case 'L': lex_fs_list(); break;
            
            default: break;
         }
      }
   }
   else {
      lex_fs_help();
   }
}

void lex_mkdir(int argc, char* argv[])
{
   
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static void lex_fs_format(char* point, char* fs)
{  
   if(point == NULL || fs == NULL) {
      printf("Missing or invalid parameter\n");
      return;
   }
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

static void lex_fs_dirlist(char* path)
{
   if(path == NULL) {
      printf("Missing or invalid parameter\n");
      return;
   }
   
   /* Split the path in the form "Point:Path" */
   char** split_path = (char**) malloc(2);
   int halves = split(':',' ',path,split_path);
   if(halves != 2) {
      printf("Paths must be of the form `Mountpoint:path'\n");
      goto exit2;
   }
   
   /* Extract the two components of the path */
   char* point = (char*) malloc(strlen(split_path[0])+1);
   strcpy(point,split_path[0]);
   char* real_path = (char*) malloc(strlen(split_path[1])+1);
   strcpy(real_path, split_path[1]);
   
   /* Aquire the device to be accessed */
   device_t device = NO_DEV;
   volume_t* vol;
   foreach(i, mounted_volumes) {
      if(strcmp(point, mountpoints[i].point) == 0) {
         device = mountpoints[i].device;
         vol = mountpoints[i].vol;
         break;
      }
   }
   if(device == NO_DEV) {
      printf("%s is not mounted\n",point);
      goto exit1;
   }
   
   /* Read the actual list */
   char** list = (char**) malloc(64);
   
   int entries = anica_list_contents(vol, path, list);
   if(entries < 0) {
      printf("Path not found\n");
      goto exit1;
   }
   int tabsize = vga_tabsize(0);
   vga_tabsize(10);
   foreach(i, entries) {
      if(list[i] != NULL) printf("%s\t",list[i]);
   }
   printf("\n");
   vga_tabsize(tabsize);
   
   exit1:
   free(point);
   free(real_path);
   exit2:
   foreach(i, halves) free(split_path[i]);
   free(split_path);
   return;
}

static void lex_fs_new(char* obj, char* path)
{
   if(obj == NULL || path == NULL) {
      printf("Missing or invalid parameter\n");
      return;
   }
   
   /* Split the path in the form "Point:Path" */
   char** split_path = (char**) malloc(2);
   int halves = split(':',' ',path,split_path);
   if(halves != 2) {
      printf("Paths must be of the form `Mountpoint:path'\n");
      goto exit2;
   }
   
   /* Extract the two components of the path */
   char* point = (char*) malloc(strlen(split_path[0])+1);
   strcpy(point,split_path[0]);
   char* real_path = (char*) malloc(strlen(split_path[1])+1);
   strcpy(real_path, split_path[1]);
   
   /* Aquire the device to be accessed */
   device_t device = NO_DEV;
   volume_t* vol;
   foreach(i, mounted_volumes) {
      if(strcmp(point, mountpoints[i].point) == 0) {
         device = mountpoints[i].device;
         vol = mountpoints[i].vol;
         break;
      }
   }
   if(device == NO_DEV) {
      printf("%s is not mounted\n",point);
      goto exit1;
   }
   
   /* Determine the object to be created */
   if(strcmp(obj, "dir") == 0) {
      /* TODO: Do not automatically assume floppy drive */
      if(!anica_mkdir(vol, real_path)) {
         printf("Could not create directory\n");
         goto exit1;
      }
   }
   
   exit1:
   free(point);
   free(real_path);
   exit2:
   foreach(i, halves) free(split_path[i]);
   free(split_path);
   return;
}

static void lex_fs_list(void)
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

static void lex_fs_mount(char* dev, char* point)
{
   if(dev == NULL || point == NULL) {
      printf("Missing or invalid parameter\n");
      return;
   }
   
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

static void lex_fs_unmount(char* point)
{
   if(point == NULL) {
      printf("Missing or invalid parameter\n");
   }
   
   foreach(i, mounted_volumes) {
      if(strcmp(mountpoints[i].point, point) == 0) {
         unmount(mountpoints[i].vol);
         
         mountpoints[i] = mountpoints[--mounted_volumes];
         return;
      }
   }
   
   printf("`%s' does not appear to be an active mount point\n",point);
}

static void lex_fs_help(void)
{
   printf("Usage: fs :[opt...] [val...]  - Manage a filesystem\n");
   printf(" ?                       Print this help text\n");
   printf(" f [point] [filesystem]  Formats a device with the given filesystem\n");
   printf(" c [param]               Manage filesystem caching\n");
   printf(" d [device] [sector]     Dump the raw data from the given sector\n");
   printf(" m [device] [point]      Mount a filesystem at a given point\n");
   printf(" u [point]               Unmount a filesystem\n");
   printf(" l                       List currently mounted devices\n");
   
   printf("Use `fs :[opt] help' for details on a particular command\n");
}

static void lex_fs_dump(char* fmt, char* sec)
{
   if(fmt == NULL || sec == NULL) {
      printf("Missing or invalid parameter\n");
      return;
   }
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


