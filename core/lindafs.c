#define LINDAFS_C_SOURCE
// ==========================================================================
// *   Avian Kernel   Bryan Webb (C) 2016                                   *
// *    File:          /core/lindafs.c                                      *
// *    Purpose:       Implementation of the Linda FS                       *
// ==========================================================================

#include <lindafs.h>
#include <floppy.h>
#include <stdlib.h>
#include <util.h>
#include <mmap.h>

/* ======================================================================== */
/*           Private function prototypes                                    */
/* ======================================================================== */

static volume_t* linda_format_sb(size_t,size_t,size_t,size_t);
static bool 	 linda_mkdir(volume_t*,const char[12], word);
static dword 	 linda_find_block(volume_t*, size_t);
static int 	    linda_read_itable(volume_t*, lfs_table_t*, byte);
static int 	    linda_write_itable(volume_t*, lfs_table_t*, byte);

/* ======================================================================= */
/*           Public API functions
/* ======================================================================= */

bool linda_read_superblock(byte device, volume_t* superblock)
{
	/* Read the first sector off the disk */
	byte* mbr = (byte*) malloc(512);
	floppy_read_block(0, mbr, 512);
	
	memcpy(superblock, mbr, sizeof(volume_t));

	free(mbr);
	return true;
}

bool linda_format_device(size_t sec, size_t bps, size_t res, size_t tbl)
{
   /* Format the superblock */
   notify("Formatting superblock\n");
	volume_t* vol = linda_format_sb(sec, bps, res, tbl);
	
	/* Init index table(s) */
	
	/* Create blank table entry */
	notify("Initializing index tables\n");
	lfs_entry_t* blank = new(lfs_entry_t);
	blank->type = LINDA_FREE;
	blank->size = 1024;
	blank->addr = 0xFFFFFFFF;
	
	/* Create blank table */
	lfs_table_t* table = new(lfs_table_t);
	table->size = 0;
	table->end = 0xED;
	
	/* Populate blank table with blank entries */
	for(int i=0; i<vol->table_size; i++) {
	   table->entry[i] = *(blank);
	}
	
	/* Populate disk with blank tables */
   for(int i=0; i<vol->tables; i++) {
      linda_write_itable(vol, table, i);
   }
	
	/* Create root directory */
	notify("Creating root directory\n");
	if(!linda_mkdir(vol, "ROOT", 0)) {
	   notify("Failed!\n");
	}
	
	
	return true;
	
}

static volume_t* linda_format_sb(size_t sec, size_t bps, size_t res, size_t tbl)
{
	byte* mbr = (byte*) malloc(512);
	for(each(i,512)) mbr[i] = 0;
	const byte jump[3] = { 0xeb, 0x3c, 0x90 };
	
	//notify("Formatting superblock\n");
	volume_t* superblock = new(volume_t);
	memcpy(superblock->jump, jump, 3);
	memcpy(superblock->uuid, "LindaFS", 7);
	memcpy(superblock->label, "(No Label      )", 16);
	superblock->volume_size = sec;
	superblock->sector_size = bps;
	superblock->reserved = res;
	superblock->table_addr = res + 1;
	superblock->tables = tbl;
	superblock->table_size = 85;
	superblock->root = 0;
	superblock->entries = 0;
	
	memcpy(mbr, superblock, sizeof(volume_t));

	if(!floppy_write_block(0,mbr,512)) { // Write MBR to floppy
	   notify("Failed to write superblock!\n");
	}
	free(mbr);
	return superblock;
}

static bool linda_mkdir(volume_t* vol, const char name[12], word parent)
{
   bool status = true;
   /* Create the directory node */
   //notify("Creating directory node\n");
	lfs_dir_t* dir = new(lfs_dir_t);
	memcpy(dir->name, name, 12);
	dir->permit = 0xC0DE; 
	dir->parent = parent;
	dir->self = vol->entries;  
	dir->size = 0;
	
	/* Increment the entries counter in superblock */
	vol->entries++;
	
	/* Update superblock on disk */
	//notify("Updating superblock\n");
	byte* superblock = (byte*) malloc(512);
	status = status && floppy_read_block(0, superblock, 512);
	memcpy(superblock, vol, sizeof(volume_t));
	status = status && floppy_write_block(0, superblock, 512);
	free(superblock);
	
	/* Find byte address where this entry can fit */
	dword daddress = linda_find_block(vol, sizeof(lfs_dir_t));
	dword dsector = daddress / vol->sector_size; // Sector number of address
	dword doffset = daddress % vol->sector_size; // Offset from sector start
	
	/* Write the directory node to disk */
	//notify("Writing node to disk\n");
	byte* dblock = (byte*) malloc(512);
	status = status && floppy_read_block(dsector, dblock, 512);
	memcpy(dblock+doffset, dir, sizeof(lfs_dir_t));
	status = status && floppy_write_block(dsector, dblock, 512);
	free(dblock);  // De-allocate block buffer
	
	/* Load the correct table sector */
	//notify("Reading index table\n");
	lfs_table_t* itable = new(lfs_table_t);
	byte table = vol->entries / vol->table_size;
	linda_read_itable(vol, itable, table);
	
	/* Create table entry for the directory */
	//notify("Adding node entry\n");
	lfs_entry_t* entry = new(lfs_entry_t);
	entry->type = LINDA_DIR;
	entry->size = sizeof(lfs_dir_t);
	entry->addr = daddress;
	
	/* Add the entry to the table */
	itable->entry[itable->size] = *(entry);
	itable->size++;
	free(entry);
	
	/* Update index table on disk */
	//notify("Writing index table to disk\n");
	linda_write_itable(vol, itable, table);
	
	/* All done! Erase allocated memory and exit */
	//notify("Done\n");
	free(dir);
	free(itable);
	
	return status;
	
}

/**
 *	 Reads/writes the given itable from the volume into the provided
 *  table pointer. If the table is read correctly, then the 
 *  function returns the number of elements in the table. If
 *  the function cannot read the table, a value of -1 is
 *  returned. 
 */
static int linda_read_itable(volume_t* vol, lfs_table_t* itable, byte table)
{
	if(table > vol->tables) return LINDA_FSERR;
	int result = LINDA_IOERR;
	byte* block = (byte*) malloc(512);
	
	if(floppy_read_block(vol->table_addr + table, block, 512)){
		result = LINDA_OK;
		memcpy(itable, block, 512);
	}
	free(block);
	return result;
}

static int linda_write_itable(volume_t* vol, lfs_table_t* itable, byte table)
{
   if(table > vol->tables) return LINDA_FSERR;
	int result = LINDA_IOERR;
	byte* block = (byte*) malloc(512);
	memcpy(block,itable,512);
	
	if(floppy_write_block(vol->table_addr + table, block, 512)){
		result = LINDA_OK;
	}
	free(block);
	return result;
}

/**
 *	 Searches for a space at least the size of the given
 *  size (in bytes), based on the entries in the index
 *  table. If a block is found, the address is returned.
 *  If not, a value of 0 is returned.
 */
static dword linda_find_block(volume_t* vol, size_t size)
{
	return 512*3;
}







