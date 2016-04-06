#define FAT_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/filesystem.c
//      Purpose:         Filesystem abstraction interface
// ======================================================================== */

#include <filesystem.h>

#include <stdlib.h>
#include <errors.h>
#include <string.h>

#include <floppy.h>

enum __PARTITIONS
{
   fda0, fda1, fda2, fda3,
   fdb0, fdb1, fdb2, fdb3,
};

static char* device_map[] = {
   "fda", "fdb", "fdc", "fdd",
   "hda", "hdb", "hdc", "hdd",
   "sda", "sdb", "sdc", "sdd",
   
   "ERR"
};

static char* filesystem_map[] = {
   "AnicaFS", "Ext2", "FAT12", "FAT16",
};

char* dev_to_str(device_t dev)
{
   return device_map[dev];
}

char* fs_to_str(format_t fs)
{
   return filesystem_map[fs];
}

device_t str_to_dev(char* dev)
{
   foreach(i, NO_DEV) {
      if(strcmp(dev, device_map[i]) == 0) {
         return i;
      }
   }
   return NO_DEV;
}

format_t str_to_fs(char* fs)
{
   foreach(i, NO_FS) {
      if(strcmp(fs, filesystem_map[i]) == 0) {
         return i;
      }
   }
   return NO_FS;
}

char* read_fs(device_t device)
{
   if(device == fda) {
      byte* mbr = (byte*) malloc(512);
      floppy_read_block(0, mbr, 512);
      if(mbr[38] == 0x28 || mbr[38] == 0x29) {
         free(mbr);
         return "FAT16";
      }
      else if(*(dword*)(mbr+3) == 1667853889) {
         free(mbr);
         return "AnicaFS";
      }
      else if(mbr[66] == 0x28 || mbr[99] == 0x29) {
         free(mbr);
         return "FAT32";
      }
      else {
         floppy_read_block(2, mbr, 512);
         if(*(word*)(mbr+56) == 0xef53) {
            free(mbr);
            return "Ext2";
         }
         else {
            free(mbr);
            return "Unknown";
         }
      }
   }
   else {
      return NULL;
   }
}

void format_device(device_t dev, format_t fs)
{
   if(dev == fda && fs == AnicaFS) {
      anica_format_device(2880,512,0);
   }
}

volume_t* mount(device_t device)
{  
   volume_t* volume = new(volume_t);
   anica_read_superblock(device, &volume->sb);
   volume->itable = (aentry_t*) malloc(volume->sb.table_size * 8);
   anica_read_itable(volume);
   
   return volume;
}

file_t* open(volume_t* device, const char path[], byte mode)
{  

   file_t* file = new(file_t);
   anode_t* filenode = new(anode_t);
   file->node = filenode;
   file->vol = device;
   
   anica_open_file(device, path, mode, filenode);
   
   return file;
}

size_t read(file_t* file)
{  

   anica_read_file(file->vol, file->data, file->node);
   return 0;
}

size_t write(file_t* file, void* data)
{  

   byte* bdata = (byte*) data;
   anica_write_file(file->vol, bdata, file->node);
   return 0;
}

void close(file_t* file)
{  

   free(file);
}

void unmount(volume_t* volume)
{  

   anica_write_superblock(0, &volume->sb);
   anica_write_itable(volume);
   free(volume->itable);
   free(volume);
}

int ext2_read_sector(byte device, dword sector, byte* buffer)
{
   return device?1:floppy_read_block(sector, buffer, 512);
}

int ext2_write_sector(byte device, dword sector, byte* buffer)
{
   return device?1:floppy_write_block(sector, buffer, 512);
}








/* End File Allocation Table driver */