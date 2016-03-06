#ifndef LINDAFS_H_INCLUDED
#define LINDAFS_H_INCLUDED

// ========================================================================= */
//	   Avian Kernel   Bryan Webb (C) 2016
//	   File:          avian/drivers/lindafs.h
//	   Purpose:	      Header for the Linda filesystem
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
	LINDA_DIR  = 0xd,	// Directory node
	LINDA_FILE = 0xf,	// File node
	LINDA_DATA = 0xc,	// Cluster of data
	LINDA_FREE = 0xa, // Available space

};

enum __LINDA_ERRORS
{
	LINDA_OK = 0,	   // Everything went OK
	LINDA_FSERR = 1,	// Some sort of filesystem error
	LINDA_NOFILE = 2, // Could not find file
	LINDA_NODIR = 3, 	// Could not find directory
	LINDA_IOERR = 4,	// Error reading or writing to device
	
	LINDA_ERR = 255,  // Some unknown error
};

/**
 *		Structure containing the Linda FS superblock data
 *		The superblock is a chunk of data loaded from the
 *		first sector of a partition that provides information
 *		about the data. 
 */
typedef struct __LINDA_SUPERBLOCK
{	
	byte   jump[3];		// Jump instructions
	char	 uuid[7];		// "LindaFS"
	char	 label[16]; 	// String containing the filesystem label
	dword  volume_size;	// Size of volume in blocks
	dword  sector_size;	// Size of sector in bytes (should be 512)
	dword  reserved;		// Number of reserved sectors (for bootloader)
	dword  table_addr;	// Sector number of the first index table
	byte   tables;       // Number of index tables
	dword  table_size;	// Number of indexes per table
	dword  root;			// Table index of the root directory (should be 0)
	dword  entries;		// Number of index entries
	
} FLAT volume_t;

/**	
 *		The Index Table is a list of addresses (and sizes) of
 *		each unit of data, whether it be a directory, file, or
 *		actual block of file content.
 *
 *		Each table entry is 6 bytes - 
 *			4 bits to identify the data type,
 *			12 bits that hold the size of the data in bytes
 *			32 bits that hold the byte address of the data itself
 *
 *    Since each entry is 6 bytes, a 512-byte table can store up
 *    to 85 entries, leaving two bytes to contain other table data
 */
 
typedef struct __LINDA_ENTRY
{
	byte 	type: 6;
	word	size: 10; // Max continuous data size is 1 KiB
	dword	addr;

} FLAT lfs_entry_t;

typedef struct __LINDA_INDEX_TABLE
{
	byte size;             // Number of entries
	lfs_entry_t entry[85]; // List of entries
	byte end;              // 0xED end byte signature

} FLAT lfs_table_t;

typedef struct __LINDA_DIRNODE
{
	byte name[12];	  // ASCII Name of directory (a-Z, 0-9 only)
	word permit;	  // Directory access permissions
	word parent;     // Table index of parent directory
	word self;	     // Table index of THIS directory
	byte size;		  // Number of directory contents

} FLAT lfs_dir_t;

// ======================================================================= */
//           Public API functions
// ======================================================================= */

bool linda_read_superblock(byte device, volume_t* superblock);
bool linda_format_device(size_t, size_t, size_t, size_t);














#endif