#ifndef ANICAFS_H_INCLUDED
#define ANICAFS_H_INCLUDED

// ========================================================================= */
//    Avian Kernel      Bryan Webb (C) 2016
//    File:             avian/include/anicafs.h
//    Purpose:          Header for the Linda filesystem
//
//    AnicaFS - Allocation of Nodes by Indexed Cluster Addresses 
//
// ========================================================================= */

#include <stdlib.h>

enum __ANICA_TABLE_TYPE
{
   ANICA_DIR = 'D',
   ANICA_FILE = 'F',
   ANICA_DATA = 'C',
   ANICA_FREE = 'A',
};

enum __ANICA_MODES
{
   ANICA_READ,
   ANICA_WRITE,
   ANICA_TRUNC,
   ANICA_INS,
};

enum __ANICA_ERRORS
{
   ANICA_OK     = 0,    // Everything went OK
   ANICA_FSERR  = 1,    // Some sort of filesystem error
   ANICA_NOFILE = 2,    // Could not find file
   ANICA_NODIR  = 3,    // Could not find directory
   ANICA_IOERR  = 4,    // Error reading or writing to device
   
   ANICA_ERR = 255,  // Some unknown error
};

/**   
 *    The Index Table is a list of addresses (and sizes) of
 *    each unit of data, whether it be a directory, file, or
 *    actual block of file content. On the disk, the index table
 *    is a long list of consecutive entries, with each entry being
 *    8 bytes (64 entries per sector). When nodes and clusters refer
 *    to other nodes or clusters, this reference is by the index of
 *    the sought-after node in the table. When a superblock is
 *    loaded from disk, the index table is loaded into memory, and
 *    the superblock records a pointer to that memory address.
 *
 *    Each table entry can address up to 4 GiB of space, with a
 *    maximum continuous data size of 4 KiB. One sector of entries
 *    can manage a maximum of 4 MiB of data, assuming each entry uses
 *    the maximum cluster size of 4 KiB. I think these numbers are wrong...
 *
 */
 
typedef struct __ANICA_ENTRY
{
   byte    type;    // Type of data (file, dir, etc)
   word    size;    // Size of data in bytes
   addr_t  addr;    // Byte address of cluster
   char    end;     // Ending byte

} packed aentry_t;

typedef struct __ANICA_NODE
{
   char name[12];    // ASCII Name of node
   word permit;      // Access permissions
   index_t parent;   // Index of parent directory
   index_t self;     // Index of this object
   index_t data;     // Index of content data
   char end;

} packed anode_t;

/**
 *      Structure containing the Linda FS superblock data
 *      The superblock is a chunk of data loaded from the
 *      first sector of a partition that provides information
 *      about the data. 
 *
 */
typedef struct __ANICA_SUPERBLOCK
{
   char   jump[3];   
   char   uuid[7];      // "AnicaFS"
   char   label[16];    // String containing the filesystem label
   dword  volume_size;  // Size of volume in blocks
   dword  sector_size;  // Size of sector in bytes (should be 512)
   dword  hidden;       // Number of sectors BEFORE the superblock
   dword  reserved;     // Number of reserved sectors before index table
   dword  table_addr;   // Sector number of the first index table
   dword  table_size;   // Maximum number of table entries
   dword  root;         // Table index of the root directory (should be 0)
   dword  entries;      // Number of index entries
   
} packed lsuper_t;

typedef struct __ANICA_VOLUME
{
   lsuper_t sb;
   aentry_t *itable;

} packed volume_t;

// ======================================================================= //
//           Public API functions                                          //
// ======================================================================= //

bool anica_mkdir(volume_t* vol, const char path[]);
bool anica_read_superblock(byte device, lsuper_t* superblock);
bool anica_write_superblock(byte device, lsuper_t* superblock);
bool anica_format_device(size_t, size_t, size_t);
int  anica_open_file(volume_t* vol, const char path[], byte mode, anode_t* file);
int  anica_write_file(volume_t* vol, byte* data, anode_t* node);
int  anica_read_file(volume_t* vol, byte* data, anode_t* node);

int  anica_read_itable(volume_t*);
int  anica_write_itable(volume_t*);









#endif