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
   ANICA_DIR,
   ANICA_FILE,
   ANICA_DATA,
   ANICA_SYS,
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

enum __ANICA_DEFAULTS
{
   ANICA_MAX_PATH = 64, // Maximum depth of paths
   ANICA_MIN_SIZE = 32, // Default size of new files
   ANICA_MAX_NAME = 12, // Maximum length of file/dir names
};

/**   
 *    Anica_Documentation:    
 *    The Index Table is a list of addresses (and sizes) of
 *    each unit of data, whether it be a directory, file, or
 *    actual block of file content. On the disk, the index table
 *    is a long list of consecutive entries, with each entry being
 *    8 bytes (64 entries per sector). When nodes and clusters refer
 *    to other nodes or clusters, this reference is by the index of
 *    the sought-after node in the table. When a superblock is
 *    loaded from disk, the index table is loaded into memory, and
 *    the superblock records a pointer to that memory address.
 */

typedef struct __ANICA_ENTRY
{
   dword    type:    2;    // Entry type
   dword    addr:    30;   // Node address
   index_t  parent:  16;   // Parent index
   word     size:    16;   // Node size

} packed aentry_t;

typedef struct __ANICA_NODE
{
   char name[16];    // ASCII Name of node
   word permit;      // Access permissions
   index_t self;     // Index of this node's table entry
   index_t data;     // Index of this node's data entry
   byte zero[10];    // Unused data

} packed anode_t;

/**
 *      Structure containing the AnicaFS superblock data
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
int  anica_list_contents(volume_t* vol, const char path[], char** list);

int  anica_read_itable(volume_t*);
int  anica_write_itable(volume_t*);









#endif