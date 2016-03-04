#define LINDAFS_C_SOURCE
// ==========================================================================
// *   Avian Kernel   Bryan Webb (C) 2016                                   *
// *    File:          /core/lindafs.c                                      *
// *    Purpose:       Implementation of the Linda FS                       *
// ==========================================================================

#include <lindafs.h>
#include <floppy.h>
#include <stdlib.h>
#include <string.h>
#include <mmap.h>

typedef union __LINDA_SUPERBLOCK_B
{
   volume_t vol;
   byte array[sizeof(volume_t)];

} FLAT bytes_sb_t;

typedef union __LINDA_ENTRY_B
{
   lfs_entry_t entry;
   byte array[sizeof(lfs_entry_t)];

} FLAT bytes_entry_t;

typedef union __LINDA_TABLE_B
{
   lfs_table_t table;
   byte array[sizeof(lfs_table_t)];

} FLAT bytes_table_t;

typedef union __LINDA_DIR_B
{
   lfs_dir_t dir;
   byte array[sizeof(lfs_dir_t)];
   
} FLAT bytes_dir_t;

/* ======================================================================== */
/*           Private function prototypes                                    */
/* ======================================================================== */

static void linda_format_sb(volume_t*, size_t,size_t,size_t,size_t);
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
	bytes_sb_t sb;
	
	memcpy(sb.array, mbr, sizeof(volume_t));
	*(superblock) = sb.vol;

	free(mbr);
	return true;
}

bool linda_format_device(size_t sec, size_t bps, size_t res, size_t tbl)
{
   /* Format the superblock */
   notify("Formatting superblock\n");
	volume_t* vol = new(volume_t);
	linda_format_sb(vol, sec, bps, res, tbl);
	
	/* Init index table(s) */
	
	/* Create blank table entry */
	notify("Initializing index tables\n");
	
	lfs_entry_t blank;
	blank.type = LINDA_FREE;
	blank.size = 0;
	blank.addr = 0;
	
	/* Create blank table */
	lfs_table_t table;
	table.size = 0;
	table.end = 0xED;
	
	/* Populate blank table with blank entries */
	for(int i=0; i<vol->table_size; i++) {
	   table.entry[i] = blank;
	}
	
	/* Populate disk with blank tables */
   for(int i=0; i<vol->tables; i++) {
      linda_write_itable(vol, &table, i);
   }
	
	/* Create root directory */
	notify("Creating root directory\n");
	linda_mkdir(vol, "ROOT", 0);
	
	free(vol);
	return true;
	
}

static void linda_format_sb(volume_t* superblock, size_t sec, size_t bps, size_t res, size_t tbl)
{
	byte* mbr = (byte*) malloc(512);
	foreach(i,512) mbr[i] = 0;
	const byte jump[3] = { 0xeb, 0x3c, 0x90 };
	
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
   floppy_write_block(0,mbr,512);
   
	free(mbr);
}

static inline bool linda_mkdir(volume_t* vol, const char name[12], word parent)
{
   bool status = true;
   
   /* Create the directory node */
	lfs_dir_t dir;
	
	char* _name = "            ";
	memcpy(_name, name, 12);
	memcpy(dir.name, name, 12);
	dir.permit = 0xC0DE; 
	dir.parent = parent;
	dir.self = vol->entries;  
	dir.size = 0;
	
	/* Increment the entries counter in superblock */
	vol->entries++;
	
	/* Update superblock on disk */
	byte* superblock = (byte*) malloc(512);
	floppy_read_block(0, superblock, 512);
	memcpy(superblock, vol, sizeof(volume_t));
   floppy_write_block(0, superblock, 512);
	free(superblock);
	
	/* Find byte address where this entry can fit */
	dword daddress = linda_find_block(vol, sizeof(lfs_dir_t));
	dword dsector = daddress / vol->sector_size; // Sector number of address
	dword doffset = daddress % vol->sector_size; // Offset from sector start
	
	/* Write the directory node to disk */
	byte* dblock = (byte*) malloc(512);
	floppy_read_block(dsector, dblock, 512);
	memcpy(dblock+doffset, &dir, sizeof(lfs_dir_t));
	floppy_write_block(dsector, dblock, 512);
	free(dblock);  // De-allocate block buffer
	
	/* Load the correct table sector */
	lfs_table_t itable;
	byte table = vol->entries / vol->table_size;
	linda_read_itable(vol, &itable, table);
	
	/* Create table entry for the directory */
	lfs_entry_t entry;
	entry.type = LINDA_DIR;
	entry.size = sizeof(lfs_dir_t);
	entry.addr = daddress;
	
	/* Add the entry to the table */
	itable.entry[itable.size] = entry;
	itable.size++;
	
	/* Update index table on disk */
	linda_write_itable(vol, &itable, table);
	
	/* All done! Erase allocated memory and exit */
	//notify("Done\n");
	//free(dir);
	//free(itable);
	
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
	if(table > vol->tables) {
	   print("Invalid table\n");
	   return LINDA_FSERR;
	}
	int result = LINDA_IOERR;
	byte* block = (byte*) malloc(512);
	
	floppy_read_block(vol->table_addr + table, block, 512);
	result = LINDA_OK;
	memcpy(itable, block, 512);
	
	free(block);
	return LINDA_OK;
}

static int linda_write_itable(volume_t* vol, lfs_table_t* itable, byte table)
{
	int result = LINDA_IOERR;
	byte* block = (byte*) malloc(512);
	
	memcpy(block,itable,512);
	floppy_write_block(vol->table_addr + table, block, 512);
	
	free(block);
	return LINDA_OK;
}

/**
 *	 Searches for a space at least the size of the given
 *  size (in bytes), based on the entries in the index
 *  table. If a block is found, the address is returned.
 *  If not, a value of 0 is returned.
 */
static dword linda_find_block(volume_t* vol, size_t size)
{
	return (dword)512*3;
}







