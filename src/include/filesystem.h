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
   
} device_t;

typedef struct __FILE
{
   lnode_t* node;     // Linda file node
   volume_t* vol;     // Linda filesystem
   byte* data;       // All the data contents

} packed file_t;

volume_t*  mount(device_t);
void       unmount(volume_t*);
file_t*    open(volume_t*, const char[], byte);
size_t     read(file_t*);
size_t     write(file_t*, void*);
void       close(file_t*);
char*      fserr(int);



#endif