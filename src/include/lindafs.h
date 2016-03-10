#ifndef LINDAFS_H_INCLUDED
#define LINDAFS_H_INCLUDED

// ========================================================================= */
//      Avian Kernel   Bryan Webb (C) 2016
//      File:          avian/drivers/anicafs.h
//      Purpose:         Header for the Linda filesystem
//
//    The LINDA Filesystem, or by its acronym,
//     "Linked and Indexed Node Directory Architecture" is a filesystem
//    similar to FAT and ext2. It is essentially a linked list of data
//    structures, but instead of nodes containing memory addresses, they
//    contain an index to a table of memory addresses. This allows the nodes
//    to store less than a quarter of the bytes that an array of addresses
//    would require. 
//
// ========================================================================= */

#include <stdlib.h>

enum __LINDA_TABLE_TYPE
{
   LINDA_DIR = 'D',
   LINDA_FILE = 'F',
   LINDA_DATA = 'C',
   LINDA_FREE = 'A',
};

enum __LINDA_BOUNDS
{
   TABLE_SIZE = 102,
};

enum __LINDA_MODES
{
   LINDA_READ,
   LINDA_WRITE,
};

enum __LINDA_ERRORS
{
   LINDA_OK = 0,      // Everything went OK
   LINDA_FSERR = 1,   // Some sort of filesystem error
   LINDA_NOFILE = 2, // Could not find file
   LINDA_NODIR = 3,    // Could not find directory
   LINDA_IOERR = 4,   // Error reading or writing to device
   
   LINDA_ERR = 255,  // Some unknown error
};

/**   
 *      The Index Table is a list of addresses (and sizes) of
 *      each unit of data, whether it be a directory, file, or
 *      actual block of file content. On the disk, the index table
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
 
typedef struct __LINDA_ENTRY
{
   char    start;
   byte    type;    // Type of data (file, dir, etc)
   byte    size;    // Size of data in clusters
   dword   addr;    // Byte address of cluster
   char    end;     // Ending byte

} FLAT lentry_t;

typedef struct __LINDA_NODE
{
   char name[12];    // ASCII Name of node
   word permit;      // Access permissions
   dword ctime;      // Creation time
   dword atime;      // Access time
   index_t parent;   // Index of parent directory
   index_t self;     // Index of this object
   index_t data;     // Index of content data
   size_t size;      // Size of content data

} FLAT lnode_t;

typedef struct __LINDA_CLUSTER
{
   byte data[31];    // 31 bytes of cluster data
   byte end;         // Byte that flags cluster info

} FLAT lcluster_t;

/**
 *      Structure containing the Linda FS superblock data
 *      The superblock is a chunk of data loaded from the
 *      first sector of a partition that provides information
 *      about the data. 
 */
typedef struct __LINDA_SUPERBLOCK
{   
   byte   jump[3];      // Jump instructions
   char   uuid[7];      // "LindaFS"
   char   label[16];    // String containing the filesystem label
   dword  volume_size;  // Size of volume in blocks
   dword  sector_size;  // Size of sector in bytes (should be 512)
   dword  reserved;     // Number of reserved sectors (for bootloader)
   dword  table_addr;   // Sector number of the first index table
   dword  table_size;   // Maximum number of table entries
   dword  root;         // Table index of the root directory (should be 0)
   dword  entries;      // Number of index entries
   
} FLAT lsuper_t;

typedef struct __LINDA_VOLUME
{
   lsuper_t sb;
   lentry_t *itable;

} FLAT volume_t;

// ======================================================================= //
//           Public API functions                                          //
// ======================================================================= //

bool anica_read_superblock(byte device, lsuper_t* superblock);
bool anica_write_superblock(byte device, lsuper_t* superblock);
bool anica_format_device(size_t, size_t, size_t);
int  anica_open_file(volume_t* vol, const char path[], byte mode, lnode_t* file);
int anica_write_file(volume_t* vol, byte* data, lnode_t* node);
int anica_read_file(volume_t* vol, byte* data, lnode_t* node);

int     anica_read_itable(volume_t*);
int     anica_write_itable(volume_t*);









#endif