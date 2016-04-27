#define FILESYSTEM_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/block/filesystem.c                                    
//    Purpose:       Filesystem access interface                    
// ======================================================================== */

#include <filesystem.h>
#include <anicafs.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

static int fs_open_anica(device_t* device, filesystem_t* fs);
static int fs_open_ext2(device_t* device, filesystem_t* fs);
static int fs_open_fat(device_t* device, filesystem_t* fs);

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

/**
 *    Avian_Documentation:
 *    Reads a filesystem on the given device, based on the given
 *    filesystem ID. The function populates the provided empty
 *    filesystem structure with the correct data.
 *
 *    If an invalid filesystem is requested, or another error
 *    occurs, the function returns a nonzero value indicating
 *    the error. On successfull completion, 0 is returned.
 */
int fs_open(byte fsid, device_t* device, filesystem_t* fs)
{
   switch(fsid)
   {
      case FS_ANICA: return fs_open_anica(device, fs);    break;
      case FS_EXT2:  return fs_open_ext2(device, fs);     break;
      case FS_FAT:   return fs_open_fat(device, fs);      break;
      default: return FS_INVALID; break;
   }
   return FS_OK;
}

int fs_close(filesystem_t* fs)
{
   return 0;
}

int fs_list_dir(filesystem_t* fs, const char path[], char* list[])
{
   switch(fs->fsid)
   {
      case FS_ANICA: return anica_list_contents(fs, path, list); break;
   }
   return FS_OK;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static int fs_open_anica(device_t* device, filesystem_t* fs)
{
   volume_t* vol = new(volume_t);
   fs->master = (void*) vol;
   fs->device = device;
   anica_read_device(fs);
   
   return FS_OK;
}

static int fs_open_ext2(device_t* device, filesystem_t* fs)
{
   return 1;
}

static int fs_open_fat(device_t* device, filesystem_t* fs)
{
   return 1;
}
