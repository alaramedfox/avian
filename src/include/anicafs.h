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
#include <device.h>
#include <filesystem.h>

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
   ANICA_OK     =  0,    // Everything went OK
   ANICA_FSERR  = -1,    // Some sort of filesystem error
   ANICA_NOFILE = -2,    // Could not find file
   ANICA_NODIR  = -3,    // Could not find directory
   ANICA_IOERR  = -4,    // Error reading or writing to device
   
   ANICA_ERR = 255,  // Some unknown error
};

enum __ANICA_DEFAULTS
{
   /* Definitions for various sizes */
   ANICA_MAX_PATH = 64, // Maximum depth of paths
   ANICA_MIN_SIZE = 32, // Default size of new files
   ANICA_MAX_NAME = 12, // Maximum length of file/dir names
   
   /* Definitions for special directories */
   ANICA_PARENT_DIR = '^',  // Name of a relative path's parent directory
   ANICA_CURRENT_DIR = '@', // Name of relative path's current directory
   ANICA_ROOT_DIR = '$', // Name of a filesystem's root directory
   
   /* Definitions for directory content suffixes */
   ANICA_FILE_ICON = '*',  // Appended to filenames
   ANICA_DIR_ICON = '/',   // Appended to directory names
   ANICA_SYS_ICON = '.',   // Appended to special things
   ANICA_LINK_ICON = '&',  // Appended to symbolic links
   
};

enum __ANICA_DEFS
{
   SOF = 0x02,   // Start of file
   LNF = 0x1a,   // Link point for more data
   EOF = 0x03,   // End of file
};

/**   
 *    Anica_Documentation:    
 *    The Index Table is a list of addresses (and sizes) of
 *    each unit of data, whether it be a directory, file, or
 *    actual block of file content. On the disk, the index table
 *    is a long list of consecutive entries, with each entry being
 *    12 bytes (128 entries per 3 sectos). When nodes and clusters refer
 *    to other nodes or clusters, this reference is by the index of
 *    the sought-after node in the table. When a superblock is
 *    loaded from disk, the index table is loaded into memory, and
 *    the superblock records a pointer to that memory address.
 */

typedef struct __ANICA_ENTRY
{
   word     type;
   index_t  parent;
   dword    addr;
   dword    size;

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
   
} packed asuper_t;

typedef struct __ANICA_VOLUME
{
   asuper_t sb;
   aentry_t *itable;

} packed volume_t;

// ======================================================================= //
//           Public API functions                                          //
// ======================================================================= //

/* Functions defined in anica/anicafs.c */
int      anica_read_device(device_t* device, volume_t* vol);
bool     anica_write_dir(filesystem_t* fs, char* name, index_t parent);
addr_t   anica_read_data(filesystem_t* fs, addr_t addr, byte* data, size_t bytes);
addr_t   anica_write_data(filesystem_t* fs, addr_t addr, byte* data, size_t bytes);
int      anica_read_path(filesystem_t* fs, char* path, anode_t* node);
int      anica_write_file(filesystem_t* fs, byte* data, anode_t* node);
int      anica_read_file(filesystem_t* fs, byte* data, anode_t* node);
int      anica_open_file(filesystem_t* fs, char* path, byte mode, anode_t* file);
int      anica_list_contents(filesystem_t* fs, char* path, char** list);
bool     anica_format_device(filesystem_t* fs, size_t sec, size_t bps, size_t res);
bool     anica_is_addr_free(filesystem_t* fs, addr_t addr);
bool     anica_block_fits(filesystem_t* fs, addr_t addr, size_t size);
addr_t   anica_find_block(filesystem_t* fs, size_t size);

/* Functions defined in anica/anica-node.c */
anode_t  anica_make_file(volume_t* vol, char* path, size_t size);
bool     anica_mkdir(volume_t* vol, char* path);
addr_t   anica_read_node(volume_t* vol, aentry_t* entry, anode_t* node);
addr_t   anica_write_node(volume_t* vol, aentry_t* entry, anode_t* node);

/* Functions defined in anica/anica-sb.c */
bool     anica_read_superblock(device_t* device, asuper_t* superblock);
bool     anica_write_superblock(device_t* device, asuper_t* superblock);
void     anica_format_sb(asuper_t* superblock, size_t sec, size_t bps, size_t res);
word     anica_find_superblock(void);

/* Functions defined in anica/anica-table.c */
int      anica_read_itable(device_t* device, volume_t* vol);
int      anica_write_itable(device_t* device, volume_t* vol);
int      anica_parent_index(volume_t* vol, char* path, char* filename);
aentry_t anica_add_entry(volume_t* vol, byte type, index_t parent, size_t size);







#endif