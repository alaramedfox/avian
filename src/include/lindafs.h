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
	LINDA_DIR,
	LINDA_FILE,
	LINDA_DATA,
	LINDA_FREE,
};

enum __LINDA_BOUNDS
{
   TABLE_SIZE = 102,
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
	addr_t ramptr;       // Pointer to a RAM address with the index table cache
	
} FLAT volume_t;

/**	
 *		The Index Table is a list of addresses (and sizes) of
 *		each unit of data, whether it be a directory, file, or
 *		actual block of file content.
 *
 *		Each table entry is 4 bytes - 
 *			2 bits to identify the data type, 
 *          (for a maximum of 4 data types)
 *			6 bits that hold the size of the data in clusters,
 *          (for a maximum continuous data size of 2kb, or 64 clusters)
 *			24 bits that hold the CLUSTER address of the data itself
 *          (for a maximum of 512 MiB addressable space)
 *       
 *
 *    Since each entry is 4 bytes, a 512-byte table can store up
 *    to 128 entries, which equates to a maximum of 256kB of
 *    indexable data. That means that one table can index 512 sectors.
 *    Therefore, a floppy will need 6 tables. 
 */
 
typedef struct __LINDA_ENTRY
{
	byte 	type: 2;   // Type of data
	byte	size: 6;   // Size of pointed-to data in clusters
	dword	addr;      // Byte address of data

} FLAT lentry_t;

typedef struct __LINDA_INDEX_TABLE
{
   char start;
	lentry_t entry[TABLE_SIZE];
	char end;
	
} FLAT ltable_t;

typedef struct __LINDA_NODE
{
   char start;
   char name[12];    // ASCII Name of node
   word permit;      // Access permissions
   dword timestamp;  // Creation time
   index_t parent;   // Index of parent directory
   index_t self;     // Index of this object
   index_t data;     // Index of content data
   byte misc[6];     // Misc. free bytes
   char end;

} FLAT lnode_t;

typedef struct __LINDA_CLUSTER
{
   byte data[31];    // 31 bytes of cluster data
   byte end;         // Byte that flags cluster info

} FLAT lcluster_t;

// ======================================================================= //
//           Public API functions                                          //
// ======================================================================= //

bool linda_read_superblock(byte device, volume_t* superblock);
bool linda_format_device(size_t, size_t, size_t, size_t);
int  linda_open_file(volume_t* vol, const char path[], byte mode, lnode_t* file);














#endif