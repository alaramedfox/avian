#ifndef LINDAFS_H_INCLUDED
#define LINDAFS_H_INCLUDED

/* ========================================================================= */
/*	   Avian Kernel   Bryan Webb (C) 2016
/*	   File:          /core/include/lindafs.h
/*	   Purpose:	      Header for the Linda filesystem
/*
/*    The LINDA Filesystem, or by its acronym,
/*     "Linked and Indexed Node Directory Architecture" is a filesystem
/*    similar to FAT and ext2. It is essentially a linked list of data
/*    structures, but instead of nodes containing memory addresses, they
/*    contain an index to a table of memory addresses. This allows the nodes
/*    to store less than a quarter of the bytes that an array of addresses
/*    would require. 
/*
/* ========================================================================= */

#include <types.h>

enum __LINDA_TABLE_TYPE
{
	LINDA_DIR  = 'D',	// Directory node
	LINDA_FILE = 'F',	// File node
	LINDA_RES  = 'R',	// Reserved data area
	LINDA_DATA = 'C',	// Data cluster

};

/*
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
	dword  root;			// Table index of the root directory (should be 0)
	
} __attribute__ ((packed)) volume_t;

/*		
 *		The Index Table is a list of addresses (and sizes) of
 *		each unit of data, whether it be a directory, file, or
 *		actual block of file content.
 *
 *		Each table entry is 7 bytes:
 *			1 byte to identify the entry type,
 *			2 bytes that holds the size of the data in bytes
 *			4 bytes that hold the byte address of the data itself
 *
 *    Since each entry is 7 bytes, a 512-byte table can store up
 *    to 73 entries, leaving one byte to signify the number of
 *    used entries.
 */
 
typedef struct __LINDA_TABLE_ENTRY
{
	byte 	type;
	word	size;
	dword	addr;

} __attribute__ ((packed)) lfs_entry_t;


typedef struct __LINDA_INDEX_TABLE
{
	byte entries;
	lfs_entry_t entry[73];

} __attribute__ ((packed)) lfs_table_t;

#define DIRNODE_SIZE		32
typedef struct __LINDA_DIRNODE
{
	byte name[12];		  // ASCII Name of directory (a-Z, 0-9 only)
	word permit;		  // Directory access permissions
	word parent_index;  // Table index of parent directory
	word this_index;	  // Table index of THIS directory
	byte size;		     // Number of directory contents
	word start;			  // Starting index of directory contents
	byte empty[11];

} __attribute__ ((packed)) lfs_dir_t;

/* ======================================================================= */
/*           Public API functions
/* ======================================================================= */

bool linda_read_superblock(byte device, volume_t* superblock);
bool linda_format_device(byte device, size_t sectors, size_t bps, byte tables);














#endif