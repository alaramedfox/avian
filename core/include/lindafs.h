#ifndef SFS_H_INCLUDED
#define SFS_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/lindafs.h
 *		Purpose:	Header for the Linda filesystem
 */

#include <types.h>

/*
 *		Structure containing the Linda FS superblock data
 *		The superblock is a chunk of data loaded from the
 *		first sector of a partition that provides information
 *		about the data. 
 */
typedef struct __LINDA_SUPERBLOCK
{
	byte	 jump[3];		// Jump opcode 
	byte	 label[16]; 	// String containing the filesystem label
	size_t volume_size;	// Size of volume in blocks
	size_t sector_size;	// Size of sector in bytes (should be 512)
	byte	 
	
	addr_t table_addr;	// Byte address of the index table
	addr_t root_addr;		// Byte address of the root directory
	

} lfs_volume_t;

/*		
 *		The Index Table is a list of addresses (and sizes) of
 *		each unit of data, whether it be a directory, file, or
 *		actual block of file content.
 *
 *		Each table entry is 8 bytes:
 *			1 byte to identify the entry type,
 *			3 bytes that holds the size of the data
 *			4 bytes that hold the byte address of the data itself
 */
 
typedef struct __LINDA_TABLE_ENTRY
{
	byte 		type: 8;
	size_t	size: 24;
	addr_t	address;

} __attribute__ ((packed)) lfs_entry_t;


typedef struct __LINDA_DIRNODE
{
	byte signature;
	byte name[8];
	byte owner;
	byte permit;
	byte entries;
	linda_dirnode_t *parent;

} lfs_dir_t;


#endif