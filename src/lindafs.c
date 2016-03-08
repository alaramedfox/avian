#define LINDAFS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                                  
//    File:          avian/lindafs.c                                     
//    Purpose:       Implementation of the Linda FS                      
// ======================================================================== */

#include <lindafs.h>
#include <stdlib.h>
#include <vga.h>
#include <util.h>
#include <string.h>
#include <floppy.h>

#include <util.h>

enum __LINDA_DEFS
{
   SOF = 0x02,   // Start of file
   LNF = 0x1a,   // Link point for more data
   EOF = 0x03,   // End of file
   
   NODE_START = '<',
   NODE_END = '>',
   TABLE_START = '{',
   TABLE_END = '}',
};

// ======================================================================== //
//           Private function prototypes                                    //
// ======================================================================== //

static void  linda_format_sb(volume_t*, size_t,size_t,size_t,size_t);
static bool  linda_mkdir(volume_t*,char*, word, ltable_t*);
static dword linda_find_block(volume_t*, ltable_t*);
static int 	 linda_read_itable(volume_t*, ltable_t*, byte);
static int 	 linda_write_itable(volume_t*, ltable_t*, byte);
static char* linda_filename(const char path[]);
static addr_t  linda_read_node(volume_t*, lentry_t*, lnode_t*);
static addr_t  linda_write_node(volume_t*, lentry_t*, lnode_t*);
static addr_t linda_read_data(volume_t*, addr_t, byte*, size_t);
static addr_t linda_write_data(volume_t*, addr_t, byte*, size_t);

// ======================================================================== //
//           Public API functions                                           //
// ======================================================================== //

bool linda_read_superblock(byte device, volume_t* superblock)
{
	byte* block = (byte*) malloc(512);
	floppy_read_block(0, block, 512);
	memcpy(superblock, block, sizeof(volume_t));
	
	free(block);
	return true;
}

bool linda_format_device(size_t sec, size_t bps, size_t res, size_t tbl)
{
   byte* zero = (byte*) calloc(512*18, sizeof(byte));
   
   foreach(i, 18) {
      notify("Zeroing first track\r");
      floppy_write_block(i, zero, 512*18);
   }
   free(zero);
   print("\n");
   

   /* Format the superblock */
   notify("Formatting superblock\n");
	volume_t* vol = new(volume_t);
	linda_format_sb(vol, sec, bps, res, tbl);
	
	/* Init index table(s) */
	
	
	
	/* Create blank table */
	ltable_t* itable = new(ltable_t);
	itable->start = '{';
	itable->end = '}';
	
	foreach(i, TABLE_SIZE) {
	   lentry_t entry;
	   entry.type = LINDA_FREE;
	   entry.size = 0;
	   entry.addr = 0;
	   
	   itable->entry[i] = entry;
	}
	
	/* Populate disk with blank tables */
   for(int i=0; i<vol->tables; i++) {
      notify("Initializing index table ("); print(itoa(i+1, DEC)); 
      print("/"); print(itoa(vol->tables,DEC)); print(")\r");
      linda_write_itable(vol, itable, i);
   }
   print("\n");
	
	/* Create root directory */
	notify("Creating root directory\n");
	linda_mkdir(vol, "ROOT", 0, itable);
	
	linda_write_itable(vol, itable, 0);
	free(itable);
	free(vol);
	return true;
	
}

int linda_open_file(volume_t* vol, const char path[], byte mode, lnode_t* file)
{
   int status;
   notify("Opening file\n");
   /* For now, assume path is in root directory */
   char* filename = linda_filename(path);
   
   print("Looking for file node named `"); print(filename); print("'\n");
   
   /* Load the index table */
   ltable_t* itable = new(ltable_t);
   linda_read_itable(vol, itable, 0);
   
   
   lentry_t* entry = new(lentry_t); // Temporary scratch entry
   lnode_t* node = new(lnode_t);    // Temporary scratch node
   
   /* Search the index table for the filename */
   foreach(i, vol->entries) {
      //notify("Open: Processing entry "); print(itoa(i+1, DEC)); print("/");
      //print(itoa(vol->entries,DEC)); print("\r");
      *(entry) = itable->entry[i];
      
      /* We found a file, let's check its name */ 
      if(entry->type == LINDA_FILE) {
         /* Load the file node into memory */
         linda_read_node(vol, entry, node);
         char* str = (char*) malloc(13);
         memcpy(str, "            ", 13);
         memcpy(str, node->name, 12);
         print("Found file node named `"); print(str); print("'\n");
         if(strcmp(filename, str) == 0) {
            /* Yay, we found the file! */
            print("Found it!\n");
            memcpy(file, node, sizeof(lnode_t));
            status = LINDA_OK;
            break;
         }
      }
   }
   print("\n");
   
   /* File was not found, so lets create it */
   if(status != LINDA_OK) {
      notify("File not found. Attempting to create\n");
   
      /* Create file node */
      node->start = '<';
      memcpy(node->name, filename, 12);
      node->parent = 0; // Still assuming root dir for now
      node->permit = 0xC0DE;
      node->self = vol->entries+1;
      node->data = vol->entries+2;
      node->end = '>';
      
      /* Copy file node to the provided node structure */
      memcpy(file, node, sizeof(lnode_t));
      
      /* Create table entry for file node */
      lentry_t* file_entry = new(lentry_t);
      file_entry->type = LINDA_FILE;
      file_entry->size = 1;
      file_entry->addr = linda_find_block(vol, itable);
      
      /* Add file entry to the index table */
      itable->entry[vol->entries] = *(file_entry);
      vol->entries++;
      
      //linda_write_itable(vol, itable, 0);
      
      /* Create table entry for file data */
      lentry_t* data_entry = new(lentry_t);
      data_entry->type = LINDA_DATA;
      data_entry->size = 4;
      data_entry->addr = linda_find_block(vol, itable);
      
      /* Add data entry to index table */
      itable->entry[vol->entries] = *(data_entry);
      vol->entries++;
      
      linda_write_itable(vol, itable, 0);
      
      /* Create actual file data */
      byte* data = (byte*) malloc(13);
      memcpy(data, " Empty file", 13);
      data[0] = SOF;
      data[12] = EOF;
      
      /* Write the new node to disk */
      linda_write_node(vol, file_entry, node);
      
      /* Write file data to disk */
      linda_write_data(vol, data_entry->addr, data, 13);
      free(data);
      
      /* Write modified index table to disk */
      linda_write_itable(vol, itable, 0);
      
      /* Write modified superblock to disk */
      byte* superblock = (byte*) malloc(512);
      memcpy(superblock, vol, sizeof(volume_t));
      floppy_write_block(0, superblock, 512);
      free(superblock);
      
      /* De-allocate used memory */
      free(data_entry);
      free(file_entry);
      
      status = LINDA_OK;
   }
   
   /* De-allocate used memory */
   free(itable);
   free(node);
   free(entry);
   
   return status;
}

// ======================================================================== //
//           Private functions                                              //
// ======================================================================== //

static addr_t linda_write_data(volume_t* vol, addr_t addr, byte* data, size_t bytes)
{
   dword sector = addr / vol->sector_size;
   dword offset = addr % vol->sector_size;
   
   byte* block = (byte*) malloc(512);
   floppy_read_block(sector, block, 512);
   memcpy(block+offset, data, bytes);
   floppy_write_block(sector, block, 512);
   free(block);
   
   return addr;
}

static addr_t linda_read_data(volume_t* vol, addr_t addr, byte* data, size_t bytes)
{
   dword sector = addr / vol->sector_size;
   dword offset = addr % vol->sector_size;
   
   byte* block = (byte*) malloc(512);
   floppy_read_block(sector, block, 512);
   memcpy(data, block+offset, bytes);
   free(block);
   
   return addr;
}

static addr_t linda_read_node(volume_t* vol, lentry_t* entry, lnode_t* node)
{  
   return linda_read_data(vol, entry->addr, (byte*)node, sizeof(lnode_t));
}

static addr_t linda_write_node(volume_t* vol, lentry_t* entry, lnode_t* node)
{
   return linda_write_data(vol, entry->addr, (byte*)node, sizeof(lnode_t));
}

static char* linda_filename(const char path[])
{
   /* file format is "NAME    EXT" */
   
   /* Calculate the length of the file name */
   int name_size = 0;
   while(path[name_size] != '.' && name_size < strlen(path)) 
      { name_size++; }
      
   /* Calculate the length of the extension */
   int ext_size = strlen(path) - (name_size+1);
   
   /* Calculate the number of padding spaces */
   int pads = 11 - (ext_size + name_size);
   
   /* Construct the new string */
   char* filename = (char*) malloc(12);
   foreach(i, name_size) {
      filename[i] = path[i];
   }
   
   foreach(i, pads) {
      filename[i+name_size] = ' ';
   }
   
   foreach(i, ext_size) {
      filename[i+name_size+pads] = path[i+name_size+1];
   }
   /* Null terminate string */
   filename[11] = 0;
   return filename;
}

static void linda_format_sb(volume_t* superblock, size_t sec, size_t bps, size_t res, size_t tbl)
{
	byte* mbr = (byte*) calloc(512, sizeof(byte));
	foreach(i,512) mbr[i] = 0;
	const byte jump[3] = { 0xeb, 0x3c, 0x90 };
	
	memcpy(superblock->jump, jump, 3);
	memcpy(superblock->uuid, "LindaFS", 7);
	memcpy(superblock->label, "(No Label      )", 16);
	superblock->volume_size = sec;
	superblock->sector_size = bps;
	superblock->reserved = res;
	superblock->table_addr = res+1;
	superblock->tables = tbl;
	superblock->table_size = TABLE_SIZE;
	superblock->root = 0;
	superblock->entries = 0;
	
	memcpy(mbr, superblock, sizeof(volume_t));
   floppy_write_block(0,mbr,512);
   
	free(mbr);
}

static bool linda_mkdir(volume_t* vol, char* name, word parent, ltable_t* itable)
{
   bool status = true;
   
   /* Create the directory node */
	lnode_t dir;
	char _name[12] = "           /";
	memcpy(_name, name, strlen(name));
	memcpy(dir.name, _name, 12);
	dir.permit = 0xC0DE;
	dir.timestamp = clock();
	dir.parent = parent;
	dir.self = vol->entries;  
	dir.data = 0;
	
	/* Create table entry for the directory */
	lentry_t entry;
	entry.type = LINDA_DIR;
	entry.size = 1;
	entry.addr = linda_find_block(vol, itable);
	itable->entry[vol->entries] = entry;
	vol->entries++;
	
	/* Update superblock on disk */
	byte* superblock = (byte*) malloc(512);
	floppy_read_block(0, superblock, 512);
	memcpy(superblock, vol, sizeof(volume_t));
   floppy_write_block(0, superblock, 512);
	free(superblock);
	
   /* Update index table on disk */
	linda_write_itable(vol, itable, 0);
	linda_write_node(vol, &entry, &dir); 
	
   return status;
	
}

/**
 *	 Reads/writes the given itable from the volume into the provided
 *  table pointer. If the table is read correctly, then the 
 *  function returns the number of elements in the table. If
 *  the function cannot read the table, a value of -1 is
 *  returned. 
 */
static int linda_read_itable(volume_t* vol, ltable_t* itable, byte table)
{
   int sector = vol->table_addr + table;
   print("Reading table from sector "); print(itoa(sector,DEC)); print("\n");
	byte* block = (byte*) malloc(512);
	floppy_read_block(sector, block, 512);
	memcpy(itable, block, 512);
	free(block);
	return LINDA_OK;
}

static int linda_write_itable(volume_t* vol, ltable_t* itable, byte table)
{
   int sector = vol->table_addr + table;
   print("Writing table to sector "); print(itoa(sector,DEC)); print("\n");
	byte* block = (byte*) malloc(512);
	memcpy(block, itable, 512);
	floppy_write_block(sector, block, 512);
	free(block);
	return LINDA_OK;
}

/**
 *	 Searches for a space at least the size of the given
 *  size (in bytes), based on the entries in the index
 *  table. If a block is found, the address is returned.
 *  If not, a value of 0 is returned.
 *
 *  If the requested size is the size of a node, then linda
 *  assumes that a node is being stored, and will prefer
 *  a space near other nodes. If not, then linda will prefer
 *  to use a space at the start of a sector.
 */
static dword linda_find_block(volume_t* vol, ltable_t* itable)
{
   dword start = 512*(vol->table_addr + vol->tables + vol->reserved);
   dword address = start;
   
   /* Sum up each cluster size */
   foreach(i, vol->entries) {
      address += itable->entry[i].size * 32;
   }
   return address;
}







