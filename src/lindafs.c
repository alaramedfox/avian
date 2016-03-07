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
};

// ======================================================================== //
//           Private function prototypes                                    //
// ======================================================================== //

static void  linda_format_sb(volume_t*, size_t,size_t,size_t,size_t);
static bool  linda_mkdir(volume_t*,char*, word);
static dword linda_find_block(volume_t*, size_t);
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
   /* Format the superblock */
   notify("Formatting superblock\n");
	volume_t* vol = new(volume_t);
	linda_format_sb(vol, sec, bps, res, tbl);
	
	/* Init index table(s) */
	
	/* Create blank table entry */
	notify("Initializing index tables\n");
	
	lentry_t blank;
	blank.type = LINDA_FREE;
	blank.size = 0;
	blank.addr = 0;
	
	/* Create blank table */
	ltable_t table;
	table.size = 0;
	table.end = 0xED;
	
	/* Populate blank table with blank entries */
	foreach(i, vol->table_size) {
	   table.entry[i] = blank;
	}
	
	/* Populate disk with blank tables */
   foreach(i, vol->tables) {
      linda_write_itable(vol, &table, i);
   }
	
	/* Create root directory */
	notify("Creating root directory\n");
	linda_mkdir(vol, "ROOT", 0);
	
	free(vol);
	return true;
	
}

int linda_open_file(volume_t* vol, const char path[], byte mode, lnode_t* file)
{
   int status;
   notify("Opening file\n");
   /* For now, assume path is in root directory */
   char* filename = linda_filename(path);
   
   notify("Open: reading index table\n");
   
   /* Load the index table */
   ltable_t* itable = new(ltable_t);
   linda_read_itable(vol, itable, 0);
   
   lentry_t* entry = new(lentry_t); // Temporary scratch entry
   lnode_t* node = new(lnode_t);    // Temporary scratch node
   
   /* Search the index table for the filename */
   foreach(i, itable->size) {
      notify("Open: Processing entry #"); print(itoa(i, DEC)); print("\n");
      *(entry) = itable->entry[i];
      
      /* We found a file, let's check its name */ 
      if(entry->type == LINDA_FILE) {
         /* Load the file node into memory */
         linda_read_node(vol, entry, node);
         
         if(strcmp(filename, node->name) == 0) {
            /* Yay, we found the file! */
            memcpy(file, node, sizeof(lnode_t));
            status = LINDA_OK;
            break;
         }
      }
   }
   
   /* File was not found, so lets create it */
   if(status != LINDA_OK) {
      notify("File not found. Attempting to create\n");
   
      /* Create file node */
      memcpy(node->name, filename, 12);
      node->parent = 0; // Still assuming root dir for now
      node->self = itable->size+1;
      node->data = itable->size+2;
      
      /* Copy file node to the provided node structure */
      memcpy(file, node, sizeof(lnode_t));
      
      /* Create table entry for file node */
      lentry_t* file_entry = new(lentry_t);
      file_entry->type = LINDA_FILE;
      file_entry->size = sizeof(lnode_t);
      file_entry->addr = linda_find_block(vol, sizeof(lnode_t));
      
      /* Add file entry to the index table */
      itable->entry[itable->size] = *(file_entry);
      itable->size++;
      vol->entries++;
      
      linda_write_itable(vol, itable, 0);
      
      /* Create table entry for file data */
      lentry_t* data_entry = new(lentry_t);
      data_entry->type = LINDA_DATA;
      data_entry->size = 13;
      data_entry->addr = linda_find_block(vol, 13);
      
      /* Add data entry to index table */
      itable->entry[itable->size] = *(data_entry);
      itable->size++;
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
   notify("Writing data to byte address ");
   print(itoa(addr, HEX));
   print("\n");
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
   notify("Reading data from byte address ");
   print(itoa(addr, HEX));
   print("\n");
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
   int pads = 12 - (ext_size + name_size);
   
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
   return filename;
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

static bool linda_mkdir(volume_t* vol, char* name, word parent)
{
   bool status = true;
   
   /* Create the directory node */
	lnode_t dir;
	
	char _name[12] = "            ";
	memcpy(_name, name, strlen(name));
	memcpy(dir.name, _name, 12);
	dir.permit = 0xC0DE; 
	dir.parent = parent;
	dir.self = vol->entries;  
	dir.data = 0;
	
	/* Increment the entries counter in superblock */
	vol->entries++;
	
	/* Update superblock on disk */
	byte* superblock = (byte*) malloc(512);
	floppy_read_block(0, superblock, 512);
	memcpy(superblock, vol, sizeof(volume_t));
   floppy_write_block(0, superblock, 512);
	free(superblock);
	
	/* Load the correct table sector */
	ltable_t itable;
	byte table = vol->entries / vol->table_size;
	linda_read_itable(vol, &itable, table);
	
	/* Create table entry for the directory */
	lentry_t entry;
	entry.type = LINDA_DIR;
	entry.size = sizeof(lnode_t);
	entry.addr = linda_find_block(vol, sizeof(lnode_t));
	
	linda_write_node(vol, &entry, &dir); 
	
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
static int linda_read_itable(volume_t* vol, ltable_t* itable, byte table)
{
   print("Reading index table "); println(itoa(table,DEC));
	byte* block = (byte*) malloc(512);
	floppy_read_block(vol->table_addr + table, block, 512);
	memcpy(itable, block, 512);
	free(block);
	return LINDA_OK;
}

static int linda_write_itable(volume_t* vol, ltable_t* itable, byte table)
{
   print("Writing index table "); print(itoa(table,DEC));
   print(" ("); print(itoa(itable->size,DEC)); print(" entries)\n");
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
 *
 *  If the requested size is the size of a node, then linda
 *  assumes that a node is being stored, and will prefer
 *  a space near other nodes. If not, then linda will prefer
 *  to use a space at the start of a sector.
 */
static dword linda_find_block(volume_t* vol, size_t size)
{
   /* Load index table */
   ltable_t* itable = new(ltable_t);
   linda_read_itable(vol, itable, 0);
   
   addr_t start = vol->sector_size * (vol->reserved + vol->tables + 1);
   addr_t end = vol->volume_size * vol->sector_size;
   addr_t addr = start;
   
   if(itable->size == 0) {
      print("There are no entries in the index table\n");
      free(itable);
      return addr;
   }
   
   while(addr < end)
   {
      foreach(i, itable->size) {
         print("Looking at table entry "); print(itoa(i,DEC)); print("\n");
         if(addr >= itable->entry[i].addr && 
            addr <= itable->entry[i].addr + itable->entry[i].size) {
         
            continue;
            
         }
         else {
            /* Found a free block! */
            notify("Found free address starting at ");
            print(itoa(addr, HEX));
            print("\n");
            free(itable);
            return (dword)addr;
         }
      }
      
      
      addr += size;
   }
   return 0;
}







