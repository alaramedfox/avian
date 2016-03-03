#define LINDAFS_C_SOURCE
/* ======================================================================== */
/*    Avian Kernel   Bryan Webb (C) 2016
/*    File:          /core/lindafs.c
/*    Purpose:       Implementation of the Linda FS
/* ======================================================================== */

#include <lindafs.h>
#include <floppy.h>
#include <stdlib.h>
#include <util.h>
#include <mmap.h>

static bool linda_format_superblock(size_t,size_t,size_t,size_t);
static bool linda_create_directory(volume_t*,const char[12], word, word);

enum __SUPERBLOCK_OFFSETS
{
	LFS_IDSTR = 3,
	LFS_LABEL = 9,
	LFS_VSIZE = 25,
	LFS_SSIZE = 29,
	LFS_TADDR = 33,
	LFS_TSIZE = 37,
	LFS_ROOTD = 38,
};

bool linda_read_superblock(byte device, volume_t* superblock)
{
	/* Read the first sector off the disk */
	byte* mbr = (byte*) malloc(512);
	floppy_read_block(0, mbr, 512);
	
	memcpy(superblock, mbr, sizeof(volume_t));

	free(mbr);
	return true;
}

#define ROOT_INDEX	0

bool linda_format_device(byte device, size_t sectors, size_t bps, byte tables)
{
	if(!linda_format_superblock(device, sectors, bps, tables)) {
		print("Failed to create superblock\n");
		return false;
	}
	
#if 0
	/* Create a table entry */
	lfs_entry_t rootentry;
	rootentry.type = LINDA_DIR;
	rootentry.size = DIRNODE_SIZE;
	rootentry.addr = rootaddr;
	
	/* Time to create the first index table */
	lfs_table_t* itable = new(lfs_table_t);
	itable->entries = 1;
	itable->entry[ROOT_INDEX] = rootentry;
	
	/* Now we write the table to the disk */
	byte* itable_sector = (byte*) malloc(512);
	for(each(i,512)) itable_sector[i] = 0;
	memcpy(itable_sector, itable, 512);
	if(!floppy_write_block(1, itable_sector, 512)) {
		print("Failed to write index table!\n");
	}
	
	free(itable_sector);
	free(root);
	free(itable);
#endif
	
	return true;
	
}

static bool linda_format_superblock(size_t s, size_t b, size_t r, size_t t)
{
	byte* mbr = (byte*) malloc(512);
	for(each(i,512)) mbr[i] = 0;
	bool status = true;
	const byte jump[3] = { 0xeb, 0x3c, 0x90 };
	
	volume_t superblock;
	memcpy(superblock.jump, jump, 3);
	memcpy(superblock.uuid, "LindaFS", 7);
	memcpy(superblock.label, "(No Label)", 10);
	superblock.volume_size = s;
	superblock.sector_size = b;
	superblock.reserved = r;
	superblock.table_addr = r + 1;
	superblock.tables = t;
	superblock.root = 0;
	
	memcpy(mbr, &superblock, sizeof(volume_t));

	status = floppy_write_block(0,mbr,512); // Write MBR to floppy
	
	free(mbr);
	return status;
}

static bool linda_create_directory(volume_t* sb, const char n[12], word pm, word pr)
{
	lfs_dir_t dir;
	dir.name = name;
	dir.permit = pm;
	dir.parent_index = pr;
	
	dir.this_index = tp;
	dir.size = 0;
	dir.start = this_index + 1;
	
	/* Calculate the byte address of the root dir */
	dword rootaddr = (tables+1) * bps;
	
	/* Now calculate the sector and offset */
	dword rsector = rootaddr / bps;
	dword roffset = rootaddr % bps;
	
	/* Add the root directory to the disk */
	byte* tempsect = (byte*) malloc(512);
	for(each(i,512)) tempsect[i] = 'F';
	
	floppy_write_block(rsector, tempsect, 512);
	
	memcpy(tempsect+roffset, root->name, 8);
	pair16_t p16;
	
	p16._word = root->permit;
	memcpy(tempsect+roffset+12, p16._byte, 2);
	
	p16._word = root->parent_index;
	memcpy(tempsect+roffset+14, p16._byte, 2);
	
	p16._word = root->this_index;
	memcpy(tempsect+roffset+16, p16._byte, 2);
	
	if(!floppy_write_block(rsector, tempsect, 512)) {
		print("Failed to write root directory!\n");
	}
	free(tempsect);
#endif
}









