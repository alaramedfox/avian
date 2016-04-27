#ifndef FILESYSTEM_H_INCLUDED
#define FILESYSTEM_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/include/filesystem.h                                    
//    Purpose:       Filesystem IO abstraction                    
// ======================================================================== */

#include <device.h>
#include <stdlib.h>
#include <stdio.h>

// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

enum __FS_STATUS
{
   FS_OK = 0, FS_INVALID = 1, 
   
   FS_MISC,
};

enum __FS_ID
{
   FS_ANICA, FS_EXT2, FS_FAT,
   
   FS_ERROR
};

typedef struct __FILESYSTEM
{
   byte fsid;           // ID value of filesystem
   device_t* device;    // Device used for IO
   void* master;        // Pointer to the filesystem's superstructure

} packed filesystem_t;

int fs_open(byte fsid, device_t* device, filesystem_t* fs);
int fs_close(filesystem_t* fs);

int fs_list_dir(filesystem_t* fs, const char path[], char* list[]);

#endif