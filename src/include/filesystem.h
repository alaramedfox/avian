#ifndef FAT_H_INCLUDED
#define FAT_H_INCLUDED
// ======================================================================== */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:            avian/include/filesystem.h
//      Purpose:         Open, close, edit files on a given volume
// ======================================================================== */

#include <stdlib.h>
#include <anicafs.h>

typedef enum __DEVICES
{
   fda=0, fdb, hda, hdb, hdc, hdd,
   sda, sdb, sdc, sdd,
   
   NO_DEV,
   
} device_t;

typedef enum __FILESYSTEM
{
   AnicaFS, Ext2, FAT12, FAT16,
   
   NO_FS,

} format_t;

typedef struct __FILE
{
   anode_t* node;     // Linda file node
   volume_t* vol;     // Linda filesystem
   byte* data;       // All the data contents

} packed file_t;

char*      read_fs(device_t);

device_t   str_to_dev(char*);
char*      dev_to_str(device_t);

format_t   str_to_fs(char*);
char*      fs_to_str(format_t);
void       format_device(device_t, format_t);
volume_t*  mount(device_t);
void       unmount(volume_t*);
file_t*    open(volume_t*, const char[], byte);
size_t     read(file_t*);
size_t     write(file_t*, void*);
void       close(file_t*);
char*      fserr(int);



#endif