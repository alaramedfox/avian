#define LINDAFS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                                  
//    File:          avian/anicafs.c                                     
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

static void  anica_format_sb(lsuper_t*, size_t,size_t,size_t);
static bool  anica_mkdir(volume_t*,char*, word);
static dword anica_find_block(volume_t*);

static char* anica_filename(const char path[]);
static addr_t  anica_read_node(volume_t*, lentry_t*, lnode_t*);
static addr_t  anica_write_node(volume_t*, lentry_t*, lnode_t*);
static addr_t anica_read_data(volume_t*, addr_t, byte*, size_t);
static addr_t anica_write_data(volume_t*, addr_t, byte*, size_t);

// ======================================================================== //
//           Public API functions                                           //
// ======================================================================== //

void anica_dump_entry(volume_t* vol, int i)
{
   vga_setcolor(0x06);
   print("Entry "); iprint(i,DEC); print(": Type=");
   switch(vol->itable[i].type)
   {
      case LINDA_DIR: print("DIR, Size="); break;
      case LINDA_FILE: print("FILE, Size="); break;
      case LINDA_DATA: print("DATA, Size="); break;
      case LINDA_FREE: print("FREE, Size="); break;
      default: print("???, Size="); break;
   }
   iprint(vol->itable[i].size, DEC); print(", Addr=");
   iprint(vol->itable[i].addr, HEX); print("\n");
   vga_setcolor(0x07);
}

bool anica_read_superblock(byte device, lsuper_t* superblock)
{
   byte* block = (byte*) calloc(512, 1);
   floppy_read_block(0, block, 512);
   memcpy(superblock, block, sizeof(lsuper_t));
   
   free(block);
   return true;
}

bool anica_write_superblock(byte device, lsuper_t* superblock)
{
   byte* block = (byte*) calloc(512, 1);
   floppy_read_block(0, block, 512);
   memcpy(block, superblock, sizeof(lsuper_t));
   floppy_write_block(0, block, 512);
   free(block);
   return true;
}

bool anica_format_device(size_t sec, size_t bps, size_t res)
{
   byte* zero = (byte*) calloc(512*18, 1);

   print("Zeroing first track\n");
   floppy_write_block(0, zero, 512*18);
   free(zero);
   
   /* Format the superblock */
   print("Formatting superblock\n");
   volume_t* vol = new(volume_t);
   anica_format_sb(&vol->sb, sec, bps, res);
   vol->itable = (lentry_t*) malloc(vol->sb.table_size * sizeof(lentry_t));
   
   /* Format reserved sectors */
   foreach(i, vol->sb.reserved) {
      byte* res = (byte*) malloc(512);
      foreach(j, 512) res[j] = '-';
      memcpy(res, "This sector is reserved for boot code", 37);
      floppy_write_block(i+1, res, 512);
      free(res);
   }
   
   /* Create root directory */
   notify("Creating root directory\n");
   anica_mkdir(vol, "ROOT", 0);
   
   anica_write_superblock(0, &vol->sb);
   anica_write_itable(vol);
   free(vol->itable);
   free(vol);
   return true;
   
}

int anica_open_file(volume_t* vol, const char path[], byte mode, lnode_t* file)
{
   int status;
   //notify("Opening file\n");
   /* For now, assume path is in root directory */
   char* filename = anica_filename(path);
   //print("Looking for entry named `"); print(filename); print("'\n");
   /* Search the index table for the filename */
   foreach(i, vol->sb.entries) {
      
      ASSERT(anica_dump_entry(vol, i));
      
      /* We found a file, let's check its name */ 
      if(vol->itable[i].type == LINDA_FILE) {
         /* Load the file node into memory */
         lnode_t node;
         anica_read_node(vol, &vol->itable[i], &node);
         ASSERT(print("Found node named `"); print(node.name); print("'\n"));
         if(strcmp(filename, node.name) == 0) {
            /* Yay, we found the file! */
            *(file) = node;
            status = LINDA_OK;
            print("Found the file!\n");
            break;
         }
      }
   }
   
   /* File was not found, so lets create it */
   if(status != LINDA_OK && mode == LINDA_WRITE) {
      ASSERT(print("File not found. Attempting to create\n"));
   
      /* Create file node */
      lnode_t* node = new(lnode_t);
      memcpy(node->name, filename, 12);
      node->parent = 0; // Still assuming root dir for now
      node->permit = 0xC0DE;
      node->self = vol->sb.entries++;
      node->data = vol->sb.entries++;
      node->ctime = clock();
      node->size = 128;
      
      /* Copy file node to the provided node structure */
      //memcpy(file, node, sizeof(lnode_t));
      *(file) = *(node);
      
      /* Create table entry for file node */
      lentry_t* file_entry = new(lentry_t);
      file_entry->start = '{';
      file_entry->type = LINDA_FILE;
      file_entry->size = 32;
      file_entry->addr = anica_find_block(vol);
      file_entry->end = '}';
      vol->itable[node->self] = *(file_entry);
      
      /* Create table entry for file data */
      lentry_t* data_entry = new(lentry_t);
      data_entry->start = '{';
      data_entry->type = LINDA_DATA;
      data_entry->size = 128;
      data_entry->addr = anica_find_block(vol);
      data_entry->end = '}';
      
      /* Add data entry to index table */
      vol->itable[node->data] = *(data_entry);
      
      /* Create actual file data */
      byte* data = (byte*) calloc(128, 1);
      memcpy(data, "Empty file", 12);
      data[12] = EOF;
      
      /* Write data to disk */
      anica_write_node(vol, file_entry, node);
      anica_write_file(vol, data, node);
      
      /* De-allocate used memory */
      free(data);
      free(data_entry);
      free(file_entry);
      free(node);
      
      status = LINDA_OK;
   }
   
   return status;
}

int anica_read_file(volume_t* vol, byte* data, lnode_t* node)
{
   size_t bytes = vol->itable[node->data].size;
   dword address = vol->itable[node->data].addr;
   byte* buffer = (byte*) malloc(bytes);
   anica_read_data(vol, address, buffer, bytes);
   for(int i=0; buffer[i] != EOF; i++) {
      data[i] = buffer[i];
   }
   free(buffer);
   node->atime = clock();
   return 0;
}

int anica_write_file(volume_t* vol, byte* data, lnode_t* node)
{  
   size_t bytes = vol->itable[node->data].size;
   dword address = vol->itable[node->data].addr;
   
   anica_write_data(vol, address, data, bytes);
   return 0;
}

// ======================================================================== //
//           Private functions                                              //
// ======================================================================== //

static addr_t anica_write_data(volume_t* vol, addr_t addr, byte* data, size_t bytes)
{
   //print("Writing "); iprint((bytes,DEC)); print("bytes to address "); println(itoa(addr,HEX));
   dword sector = addr / vol->sb.sector_size;
   dword offset = addr % vol->sb.sector_size;
   
   byte* block = (byte*) malloc(512);
   floppy_read_block(sector, block, 512);
   memcpy(block+offset, data, bytes);
   floppy_write_block(sector, block, 512);
   free(block);
   
   return addr;
}

static addr_t anica_read_data(volume_t* vol, addr_t addr, byte* data, size_t bytes)
{
   //print("Reading "); iprint(bytes,DEC); 
   //print(" bytes from address "); iprint(addr,HEX);
   //print("\n");
   dword sector = addr / vol->sb.sector_size;
   dword offset = addr % vol->sb.sector_size;
   
   byte* block = (byte*) malloc(512);
   floppy_read_block(sector, block, 512);
   memcpy(data, block+offset, bytes);
   free(block);
   
   return addr;
}

static addr_t anica_read_node(volume_t* vol, lentry_t* entry, lnode_t* node)
{  
   return anica_read_data(vol, entry->addr, (byte*)node, sizeof(lnode_t));
}

static addr_t anica_write_node(volume_t* vol, lentry_t* entry, lnode_t* node)
{
   return anica_write_data(vol, entry->addr, (byte*)node, sizeof(lnode_t));
}

static char* anica_filename(const char path[])
{
   /* Calculate the length of the file name */
   int name_size = 0;
   while(path[name_size] != '.' && name_size < strlen(path)) 
      { name_size++; }
   int ext_size = strlen(path) - (name_size+1);
   int pads = 11 - (ext_size + name_size);
   char* filename = (char*) malloc(12);
   foreach(i, name_size) { filename[i] = path[i]; }
   foreach(i, pads) { filename[i+name_size] = ' '; }
   foreach(i, ext_size) {
      filename[i+name_size+pads] = path[i+name_size+1];
   }
   /* Null terminate string */
   filename[11] = 0;
   return filename;
}

static void anica_format_sb(lsuper_t* superblock, size_t sec, size_t bps, size_t res)
{
   const byte jump[3] = { 0xeb, 0x3c, 0x90 };
   
   memcpy(superblock->jump, jump, 3);
   memcpy(superblock->uuid, "AnicaFS", 7);
   memcpy(superblock->label, "(No Label      )", 16);
   superblock->volume_size = sec;
   superblock->sector_size = bps;
   superblock->reserved = res;
   superblock->table_addr = res+1;
   superblock->table_size = 64;
   superblock->root = 0;
   superblock->entries = 0;
   
   anica_write_superblock(0, superblock);
}

static bool anica_mkdir(volume_t* vol, char* name, word parent)
{
   bool status = true;
   
   print("Creating directory node\n");
   /* Create the directory node */
   lnode_t dir;
   char _name[12] = "           /";
   memcpy(_name, name, strlen(name));
   memcpy(dir.name, _name, 12);
   dir.permit = 0xC0DE;
   dir.ctime = clock();
   dir.parent = parent;
   dir.self = vol->sb.entries++;  
   dir.data = 0;
   
   print("Creating table entry\n");
   /* Create table entry for the directory */
   lentry_t entry;
   entry.start = '{';
   entry.type = LINDA_DIR;
   entry.size = 32;
   entry.addr = anica_find_block(vol);
   entry.end = '}';
   vol->itable[dir.self] = entry;
   
   //print("Updating superblock\n");
   //anica_write_superblock(0, vol);

   anica_write_node(vol, &entry, &dir);
   
   return status;
   
}

/**
 *    Reads/writes the given itable from the volume into the provided
 *  table pointer. If the table is read correctly, then the 
 *  function returns the number of elements in the table. If
 *  the function cannot read the table, a value of -1 is
 *  returned. 
 */

int anica_read_itable(volume_t* vol)
{
   int sector = vol->sb.table_addr;
   //print("Reading table from sector "); iprint((sector,DEC)); print("\n");
   byte* block = (byte*) malloc(512);
   floppy_read_block(sector, block, 512);
   memcpy(vol->itable, block, 512);
   free(block);
   return LINDA_OK;
}

int anica_write_itable(volume_t* vol)
{
   int sector = vol->sb.table_addr;
   //print("Writing table to sector "); iprint((sector,DEC)); print("\n");
   byte* block = (byte*) malloc(512);
   memcpy(block, vol->itable, 512);
   
   if(!floppy_write_block(sector, block, 512)) {
      print("Failed to write block\n");
   }
   free(block);
   return LINDA_OK;
}

/**
 *    Searches for a space at least the size of the given
 *  size (in bytes), based on the entries in the index
 *  table. If a block is found, the address is returned.
 *  If not, a value of 0 is returned.
 *
 *  If the requested size is the size of a node, then anica
 *  assumes that a node is being stored, and will prefer
 *  a space near other nodes. If not, then anica will prefer
 *  to use a space at the start of a sector.
 */
static dword anica_find_block(volume_t* vol)
{
   dword start = (512*vol->sb.table_addr + vol->sb.table_size*sizeof(lentry_t));
   dword address = start;
   
   /* Sum up each cluster size */
   foreach(i, vol->sb.entries) {
      address += vol->itable[i].size;
   }
   return address;
}







