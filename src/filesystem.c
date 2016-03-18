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










/* End File Allocation Table driver */