#define ANICAFS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                                  
//    File:          avian/anicafs.c                                     
//    Purpose:       Implementation of the Linda FS                      
// ======================================================================== */

#include <anicafs.h>


#include <stdlib.h>
#include <vga.h>
#include <util.h>
#include <string.h>
#include <floppy.h>

#include <util.h>
#include <time.h>

enum __ANICA_DEFS
{
   SOF = 0x02,   // Start of file
   LNF = 0x1a,   // Link point for more data
   EOF = 0x03,   // End of file
};

// ======================================================================== //
//           Private function prototypes                                    //
// ======================================================================== //

static void  anica_format_sb(lsuper_t*, size_t,size_t,size_t);
static bool  anica_mkdir(volume_t*,char*, word);

static dword anica_find_block(volume_t*, size_t);
static word  anica_find_superblock(void);

static char* anica_filename(const char path[]);
static addr_t  anica_read_node(volume_t*, lentry_t*, lnode_t*);
static addr_t  anica_write_node(volume_t*, lentry_t*, lnode_t*);
static addr_t anica_read_data(volume_t*, addr_t, byte*, size_t);
static addr_t anica_write_data(volume_t*, addr_t, byte*, size_t);

// ======================================================================== //
//           Public API functions                                           //
// ======================================================================== //

#if 0
void anica_dump_entry(volume_t* vol, int i)
{  

   vga_setcolor(0x06);
   print("Entry "); iprint(i,DEC); print(": Type=");
   switch(vol->itable[i].type)
   {
      case ANICA_DIR: print("DIR, Size="); break;
      case ANICA_FILE: print("FILE, Size="); break;
      case ANICA_DATA: print("DATA, Size="); break;
      case ANICA_FREE: print("FREE, Size="); break;
      default: print("???, Size="); break;
   }
   iprint(vol->itable[i].size, DEC); print(", Addr=");
   iprint(vol->itable[i].addr, HEX); print("\n");
   vga_setcolor(0x07);
}
#endif

bool anica_read_superblock(byte device, lsuper_t* superblock)
{  

   word sb_sector = anica_find_superblock();

   byte* block = (byte*) calloc(512, 1);
   if(device == 0) floppy_read_block(sb_sector, block, 512);
   memcpy(superblock, block, sizeof(lsuper_t));
   superblock->hidden = sb_sector;
   
   free(block);
   return true;
}

bool anica_write_superblock(byte device, lsuper_t* superblock)
{  

   byte* block = (byte*) calloc(512, 1);
   if(device == 0) floppy_read_block(superblock->hidden, block, 512);
   memcpy(block, superblock, sizeof(lsuper_t));
   if(device == 0) floppy_write_block(superblock->hidden, block, 512);
   free(block);
   return true;
}

bool anica_format_device(size_t sec, size_t bps, size_t res)
{  
   
   /* Format the superblock */
   print("Formatting superblock\n");
   volume_t* vol = new(volume_t);
   anica_format_sb(&vol->sb, sec, bps, res);
   vol->itable = (lentry_t*) malloc(vol->sb.table_size * sizeof(lentry_t));
   
   /* Create root directory */
   notify(this, "Creating root directory\n");
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
   
   /* For now, assume path is in root directory */
   char* filename = anica_filename(path);
   
   /* Search the index table for the filename */
   foreach(i, vol->sb.entries) {
      
      /* We found a file, let's check its name */ 
      if(vol->itable[i].type == ANICA_FILE) {
         /* Load the file node into memory */
         lnode_t node;
         anica_read_node(vol, &vol->itable[i], &node);
         ASSERT(print("Found node named `"); print(node.name); print("'\n"));
         if(strcmp(filename, node.name) == 0) {
            /* Yay, we found the file! */
            *(file) = node;
            status = ANICA_OK;
            print("Found the file!\n");
            break;
         }
      }
   }
   
   /* File was not found, so lets create it */
   if(status != ANICA_OK && mode == ANICA_WRITE) {
      ASSERT(print("File not found. Attempting to create\n"));
   
      /* Create file node */
      lnode_t* node = new(lnode_t);
      memcpy(node->name, filename, 12);
      node->parent = 0; // Still assuming root dir for now
      node->permit = 0xC0DE;
      node->self = vol->sb.entries+1;
      node->data = vol->sb.entries+2;
      node->ctime = 0xffffffff;
      node->end = ';';
      
      /* Copy file node to the provided node structure */
      //memcpy(file, node, sizeof(lnode_t));
      *(file) = *(node);
      
      /* Create table entry for file node */
      lentry_t* file_entry = new(lentry_t);
      file_entry->type = ANICA_FILE;
      file_entry->size = sizeof(lnode_t);
      file_entry->addr = anica_find_block(vol, sizeof(lnode_t));
      file_entry->end = ';';
      vol->itable[node->self] = *(file_entry);
      vol->sb.entries++;
      
      /* Create table entry for file data */
      lentry_t* data_entry = new(lentry_t);
      data_entry->type = ANICA_DATA;
      data_entry->size = 12;
      data_entry->addr = anica_find_block(vol, 12);
      data_entry->end = ';';
      
      /* Add data entry to index table */
      vol->itable[node->data] = *(data_entry);
      vol->sb.entries++;

      
      /* Write data to disk */
      anica_write_node(vol, file_entry, node);
      char* message = new_str("Empty File");
      anica_write_file(vol, (byte*)message, node);
      
      /* De-allocate used memory */
      free(message);
      free(data_entry);
      free(file_entry);
      free(node);
      
      status = ANICA_OK;
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
   return 0;
}

int anica_write_file(volume_t* vol, byte* data, lnode_t* node)
{  

   size_t bytes = vol->itable[node->data].size;
   dword address = vol->itable[node->data].addr;
   
   size_t datasize = ptrsize(data)+2;
   /* If the file will be larger, re-allocate the file */
   if(datasize > bytes) {
      address = anica_find_block(vol, datasize);
      bytes = datasize;
      vol->itable[node->data].size = bytes;
      vol->itable[node->data].addr = address;
      anica_write_node(vol, &vol->itable[node->data], node);
   }
   data[bytes-1] = EOF;
   anica_write_data(vol, address, data, bytes);
   return 0;
}

// ======================================================================== //
//           Private functions                                              //
// ======================================================================== //

static word anica_find_superblock(void)
{  

   byte* mbr = (byte*) malloc(512);
   floppy_read_block(0, mbr, 512);
   word offset=0;
   if(mbr[0] != 'A') {
      /* Found a FAT filesystem */
      memcpy(&offset, mbr+0x13, 2);
   }
   free(mbr);
   return offset;
}

static addr_t anica_write_data(volume_t* vol, addr_t addr, byte* data, size_t bytes)
{  

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
   size_t name_size = 0;
   while(path[name_size] != '.' && name_size < strlen(path)) 
      { name_size++; }
   size_t ext_size = strlen(path) - (name_size+1);
   size_t pads = 11 - (ext_size + name_size);
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

   word start = anica_find_superblock();
   
   memcpy(superblock->uuid, "AnicaFS", 7);
   memcpy(superblock->label, "(No Label      )", 16);
   superblock->volume_size = sec;
   superblock->sector_size = bps;
   superblock->hidden = start;
   superblock->reserved = res;
   superblock->table_addr = res+start+1;
   superblock->table_size = 64;
   superblock->root = 0;
   superblock->entries = 0;
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
   dir.ctime = 0xffffffff;
   dir.parent = parent;
   dir.self = vol->sb.entries++;
   dir.data = 0;
   dir.end = ';';
   
   print("Creating table entry\n");
   /* Create table entry for the directory */
   lentry_t entry;
   entry.type = ANICA_DIR;
   entry.size = sizeof(lnode_t);
   entry.addr = anica_find_block(vol, sizeof(lnode_t));
   entry.end = ';';
   vol->itable[dir.self] = entry;

   anica_write_node(vol, &entry, &dir);
   
   return status;
   
}

/**
 *    Reads/writes the given itable from the volume into the provided
 *    table pointer. If the table is read correctly, then the 
 *    function returns the number of elements in the table. If
 *    the function cannot read the table, a value of -1 is
 *    returned. 
 */

int anica_read_itable(volume_t* vol)
{  

   int sector = vol->sb.table_addr;
   //print("Reading table from sector "); iprint((sector,DEC)); print("\n");
   byte* block = (byte*) malloc(512);
   floppy_read_block(sector, block, 512);
   memcpy(vol->itable, block, 512);
   free(block);
   return ANICA_OK;
}

int anica_write_itable(volume_t* vol)
{  

   int sector = vol->sb.table_addr;
   //print("Writing table to sector "); iprint((sector,DEC)); print("\n");
   byte* block = (byte*) malloc(512);
   memcpy(block, vol->itable, 512);
   
   floppy_write_block(sector, block, 512);
   free(block);
   return ANICA_OK;
}

/**
 *    Searches for a space at least the size of the given
 *    size (in bytes), based on the entries in the index
 *    table. If a block is found, the address is returned.
 *    If not, a value of 0 is returned.
 *
 */
 
static bool anica_is_addr_free(volume_t* vol, addr_t addr)
{  

   addr_t start, end;
   foreach(i, vol->sb.entries)
   {
      start = vol->itable[i].addr;
      end = vol->itable[i].addr + vol->itable[i].size;
      
      if(addr >= start && addr <= end) { 
         return false;
      }
      else continue;
   }
   return true;
}
 
static bool anica_block_fits(volume_t* vol, addr_t addr, size_t size)
{  
   
   size_t bs = 0;
   addr_t max = vol->sb.volume_size * vol->sb.sector_size;
   
   while(anica_is_addr_free(vol, addr+bs) && (addr < max))
   {
      ++bs;
      if(bs >= size) return true;
   }
   return false;
}

static addr_t anica_find_block(volume_t* vol, size_t size)
{  

   addr_t search_start = (512*vol->sb.table_addr + vol->sb.table_size*sizeof(lentry_t));
   addr_t max = vol->sb.volume_size * vol->sb.sector_size - vol->sb.sector_size;
   
   for(addr_t address = search_start; address < max; address+=size)
   {
      if(anica_block_fits(vol, address, size)) {
         return address;
      }
      else continue;
   }

   return 0;
}






