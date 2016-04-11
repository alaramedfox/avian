#define ANICAFS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                                  
//    File:          avian/anicafs.c                                     
//    Purpose:       Implementation of the Linda FS                      
// ======================================================================== */

#include <anicafs.h>

#include <vga.h>
#include <util.h>
#include <string.h>
#include <floppy.h>
#include <errors.h>
#include <time.h>
#include <stdio.h>

enum __ANICA_DEFS
{
   SOF = 0x02,   // Start of file
   LNF = 0x1a,   // Link point for more data
   EOF = 0x03,   // End of file
};

// ======================================================================== //
//           Private function prototypes                                    //
// ======================================================================== //

static void     anica_format_sb(lsuper_t*, size_t,size_t,size_t);
static bool     anica_write_dir(volume_t*,char*, word);
static addr_t   anica_find_block(volume_t*, size_t);
static word     anica_find_superblock(void);
static char*    anica_filename(char*);
static addr_t   anica_read_node(volume_t*, aentry_t*, anode_t*);
static addr_t   anica_write_node(volume_t*, aentry_t*, anode_t*);
static addr_t   anica_read_data(volume_t*, addr_t, byte*, size_t);
static addr_t   anica_write_data(volume_t*, addr_t, byte*, size_t);
static int      anica_read_path(volume_t*, char*, anode_t*);
static int      anica_parent_index(volume_t*, char*, char*);
static aentry_t anica_add_entry(volume_t*, byte, index_t, size_t);
static anode_t  anica_make_file(volume_t*, char*, size_t);

// ======================================================================== //
//           Public API functions                                           //
// ======================================================================== //

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
   word sb_sector = anica_find_superblock();
   //if(device == 0) floppy_read_block(superblock->hidden, block, 512);
   if(device == 0) floppy_read_block(sb_sector, block, 512);
   memcpy(block, superblock, sizeof(lsuper_t));
   //if(device == 0) floppy_write_block(superblock->hidden, block, 512);
   if(device == 0) floppy_write_block(sb_sector, block, 512);
   free(block);
   return true;
}

bool anica_format_device(size_t sec, size_t bps, size_t res)
{  
   /* Format the superblock */
   volume_t* vol = new(volume_t);
   anica_format_sb(&vol->sb, sec, bps, res);
   vol->itable = (aentry_t*) malloc(vol->sb.table_size * sizeof(aentry_t));
   
   /* Create directory structure */
   anica_write_dir(vol, "$", 0); // Root
   anica_mkdir(vol,"$/sys");
   
   anica_mkdir(vol,"$/lib");
   anica_mkdir(vol,"$/app");
   anica_mkdir(vol,"$/usr");
   
   char grub_config[] = "title Avian Kernel\nkernel 2680+160\nboot";
   
   /* Reserve file blocks for GRUB and the kernel */
   size_t kernel_size = 96 * 1024;  // 96 KiB
   size_t stage2_size = 102 * 1024; // 102 KiB
   size_t config_size = strlen(grub_config)+5;
   
   anode_t kernel = anica_make_file(vol, "$/sys/avian.bin", kernel_size);
   //anode_t stage2 = anica_make_file(vol, "$/sys/grub/stage2", stage2_size);
   //anode_t grubcf = anica_make_file(vol, "$/sys/grub/menu.lst", config_size);
   
   //vol->itable[grubcf.data].addr = 0x18E00;
   
   //anica_write_file(vol, (byte*)grub_config, &grubcf);
   
   
   vol->itable[kernel.data].addr = 1372160;  // Sector 2680
   //vol->itable[stage2.data].addr = 0x200;    // Sector 1
   
   
   anica_write_superblock(0, &vol->sb);
   anica_write_itable(vol);
   floppy_sync_cache();
   free(vol->itable);
   free(vol);
   return true;
}

bool anica_mkdir(volume_t* vol, char* path)
{
   /* Extract filename and path from given path */
   char* filename = (char*) malloc(16);
   int parent_index = anica_parent_index(vol, path, filename);
   
   bool status;
   
   if(parent_index < 0) status = false;
   else status = anica_write_dir(vol, filename, parent_index);
   
   return status;
}

/**
 *    Avian_Documentation:
 *    Searches a a path and populates the provided empty list with
 *    the names of all the contents. The function returns the total
 *    number of contents, or a negative value if an error occured.
 *
 *    The function requires that @list is not null, and has been
 *    allocated with a size large enough to hold all the contents.
 *    (Note: list[N] will be allocated in this function) 
 */
int anica_list_contents(volume_t* vol, char* path, char** list)
{
   anode_t node;
   int index = anica_read_path(vol, path, &node);
   if(index < 0) return index;
   
   size_t entries = 0;
   list[0] = (char*) malloc(3);
   list[1] = (char*) malloc(3);
   strcpy(list[entries++], "@/");
   strcpy(list[entries++], "^/");
   foreach(i, vol->sb.entries) {
      if(vol->itable[i].type != ANICA_DATA && vol->itable[i].parent == index) {
         anica_read_node(vol, &vol->itable[i], &node);
         size_t len = strlen(node.name);
         /* TODO: Fix bug where empty contents are 'discovered' */
         if(len) {
            list[entries] = (char*) calloc(len+2,1);
            memcpy(list[entries], node.name, len);
            switch(vol->itable[i].type)
            {
               case ANICA_DIR: 
                  list[entries][len] = ANICA_DIR_ICON; 
                  break;
               case ANICA_FILE: 
                  list[entries][len] = ANICA_FILE_ICON; 
                  break;
               default: 
                  list[entries][len] = ANICA_SYS_ICON; 
                  break;
            }
            entries++;
         }
      }
   }
   return entries;
}

/**
 *    Avian_Documentation:
 *    Loads a file node from the disk from the given path. The behavior
 *    varies depending on the value of mode: if the file doesn not exist,
 *    and mode is set to write, then the file is created at the specified
 *    path. Otherwise, the function returns a nonzero value that indicates
 *    the error. Errors can include an invalid path, some IO issue, bad
 *    permissions, or something else. 
 */

int anica_open_file(volume_t* vol, char* path, byte mode, anode_t* file)
{  
   int status;
   anode_t node;
   int index = anica_read_path(vol, path, &node);
   
   if(index >= 0) {
      *(file) = node;
      status = ANICA_OK;
   }
   else if(mode == ANICA_WRITE) {
      node = anica_make_file(vol, path, ANICA_MIN_SIZE);
      *(file) = node;
      status = ANICA_OK;
   }
   else {
      file = NULL;
      status = ANICA_NOFILE;
   }

   return status;
}

int anica_read_file(volume_t* vol, byte* data, anode_t* node)
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

int anica_write_file(volume_t* vol, byte* data, anode_t* node)
{  
   size_t bytes = vol->itable[node->data].size;
   addr_t address = vol->itable[node->data].addr;
   
   size_t datasize = ptrsize(data);
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

/**
 *    Avian_Documentation:
 *    Returns the table index of the final object in the path,
 *    searching through all directories in the path. The search ends
 *    when the last character in each directory substring is NOT a
 *    directory separator.
 *
 *    If the path was found, the index is returned and the located node
 *    is copied to the provided empty node structure.
 *    If not, a negative value is returned indicating the specific error.
 */
static int anica_read_path(volume_t* vol, char* path, anode_t* node)
{
   int index = vol->sb.root;
   bool found = false;
   
   char** tree = (char**) malloc(64);
   size_t depth = split('/',0,path,tree);
   foreach(level, depth)
   {
      if(strlen(tree[level]) < 1) {
         continue;  
      }
      found = false;
      
      foreach(i, vol->sb.entries) {
         aentry_t entry = vol->itable[i]; // Make a copy of the entry
         if(entry.type != ANICA_DATA && entry.parent == index) {
            anica_read_node(vol, &entry, node);
            if(strcmp(tree[level], node->name) == 0) {
               index = node->self;
               found = true;
               break;
            }
         }
      }
      if(found == false) {
         index = ANICA_NODIR;
         goto exit;
      }
   }
   exit:
   foreach(i, depth) free(tree[i]);
   free(tree);
   return index;
}

/**
 *    Avian_Documentation:
 *    Returns the itable index of the parent directory of the file
 *    or directory given by the path. If @filename is not null, then
 *    the string is set to the path name of the target in the path.
 *    For example, the path $/dir/subdir/file.ext would set @filename
 *    to `file.ext` and the function will return the index of `subdir`.
 *    If the path was not found, a negative value is returned.
 */
static int anica_parent_index(volume_t* vol, char* path, char* filename)
{
   char** tree = (char**) malloc(64);
   size_t depth = split('/',0,path,tree);
   
   printf("Path: [%s]\nSplit:\n",path);
   foreach(i, depth) printf("[%s] ", tree[i]);
   printf("\n");
   
   size_t parent_str_len = strlen(path) - strlen(tree[depth-1]) -1;
   char* parent_path = (char*) malloc(parent_str_len+1);
   memcpy(parent_path, path, parent_str_len);
   
   int parent_index = anica_read_path(vol, parent_path,NULL);
   if(filename != NULL) strcpy(filename, tree[depth-1]);
   
   foreach(i, depth) free(tree[i]);
   free(tree);
   free(parent_path);
   
   return parent_index;
}

static anode_t anica_make_file(volume_t* vol, char* path, size_t size)
{
   /* Extract filename and path from given path */
   char* filename = (char*) malloc(16);
   int parent_index = anica_parent_index(vol, path, filename);

   /* Create file node */
   anode_t node;
   strcpy(node.name, filename);
   node.permit = 0xC0DE;
   node.self = vol->sb.entries+1;
   node.data = vol->sb.entries+2;
   
   aentry_t file_entry = anica_add_entry(vol, ANICA_FILE, parent_index, sizeof(anode_t));
   aentry_t data_entry = anica_add_entry(vol, ANICA_DATA, node.data, size);

   /* Write data to disk */
   anica_write_node(vol, &file_entry, &node);
   
   free(filename);
   
   return node;
}

static aentry_t anica_add_entry(volume_t* vol, byte type, index_t parent, size_t size)
{
   aentry_t entry;
   entry.type = type;
   entry.size = size;
   entry.parent = parent;
   entry.addr = anica_find_block(vol, entry.size);
   vol->itable[vol->sb.entries++] = entry;
   return entry;
}

static word anica_find_superblock(void)
{  
   byte* mbr = (byte*) malloc(512);
   floppy_read_block(0, mbr, 512);
   word offset=0;
   if(mbr[3] != 'A') {
      /* Found a FAT filesystem */
      memcpy(&offset, mbr+0x13, 2);
   }
   free(mbr);
   //return offset;
   return 200;
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

static addr_t anica_read_node(volume_t* vol, aentry_t* entry, anode_t* node)
{  
   return anica_read_data(vol, entry->addr, (byte*)node, sizeof(anode_t));
}

static addr_t anica_write_node(volume_t* vol, aentry_t* entry, anode_t* node)
{
   return anica_write_data(vol, entry->addr, (byte*)node, sizeof(anode_t));
}

static void anica_format_sb(lsuper_t* superblock, size_t sec, size_t bps, size_t res)
{  
   word start = anica_find_superblock();
   
   strcpy(superblock->uuid, "AnicaFS");
   strcpy(superblock->label, "No Label");
   superblock->volume_size = sec;
   superblock->sector_size = bps;
   superblock->hidden = start;
   superblock->reserved = res;
   superblock->table_addr = res+start+1;
   superblock->table_size = 128;
   superblock->root = 0;
   superblock->entries = 0;
}

static bool anica_write_dir(volume_t* vol, char* name, index_t parent)
{  
   bool status = true;
   
   /* Create the directory node */
   anode_t dir;
   memset(dir.name, 0, 16);
   memcpy(dir.name, name, strlen(name));
   dir.permit = 0xC0DE;
   dir.self = vol->sb.entries++;
   dir.data = 0;
   memset(dir.zero, 'D', 10); 

   /* Create table entry for the directory */
   aentry_t entry = anica_add_entry(vol, ANICA_DIR, parent, sizeof(anode_t));
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
   size_t tsize = vol->sb.table_size * sizeof(aentry_t);
   
   byte* block = (byte*) malloc(tsize);
   floppy_read_block(sector, block, tsize);
   memcpy(vol->itable, block, tsize);
   free(block);
   return ANICA_OK;
}

int anica_write_itable(volume_t* vol)
{  
   int sector = vol->sb.table_addr;
   size_t tsize = vol->sb.table_size * sizeof(aentry_t);
   
   byte* block = (byte*) malloc(tsize);
   memcpy(block, vol->itable, tsize);
   
   floppy_write_block(sector, block, tsize);
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

   addr_t search_start = (512*vol->sb.table_addr + vol->sb.table_size*sizeof(aentry_t));
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






