#define FAT_C_SOURCE
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/filesystem.c
//      Purpose:         Filesystem abstraction interface
// ======================================================================== */

#include <filesystem.h>

#include <stdlib.h>
#include <util.h>
#include <string.h>

#include <floppy.h>

enum __PARTITIONS
{
   fda0, fda1, fda2, fda3,
   fdb0, fdb1, fdb2, fdb3,
};

volume_t* mount(device_t device)
{
   volume_t* volume = new(volume_t);
   anica_read_superblock(device, &volume->sb);
   volume->itable = (lentry_t*) malloc(volume->sb.table_size * 8);
   anica_read_itable(volume);
   return volume;
}

file_t* open(volume_t* device, const char path[], byte mode)
{
   file_t* file = new(file_t);
   lnode_t* filenode = new(lnode_t);
   file->node = filenode;
   file->vol = device;
   
   anica_open_file(device, path, mode, filenode);
   
   return file;
}

static size_t file_io(file_t* file, void* data, size_t bytes, bool read)
{
   size_t bc = 0; // Number of bytes transferred

   if(read) {
      anica_read_file(file->vol, (byte*)data, file->node);
   }
   else {
      
   }
   return bc;
}

size_t read(file_t* file, void* data, size_t bytes)
{
   return file_io(file, data, bytes, true);
}

size_t write(file_t* file, void* data, size_t bytes)
{
   return file_io(file, data, bytes, false);
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

char* fserr(int err)
{
   return "?";
}











/* End File Allocation Table driver */