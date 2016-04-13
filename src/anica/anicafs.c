#define ANICAFS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                                  
//    File:          avian/anicafs.c                                     
//    Purpose:       Implementation of the Linda FS                      
// ======================================================================== */

#include <anicafs.h>

#include <string.h>
#include <floppy.h>
#include <errors.h>
#include <stdio.h>

// ======================================================================== //
//           Public API functions                                           //
// ======================================================================== //

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
   if(vol == NULL) return -1;
   
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
int anica_read_path(volume_t* vol, char* path, anode_t* node)
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

addr_t anica_write_data(volume_t* vol, addr_t addr, byte* data, size_t bytes)
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

addr_t anica_read_data(volume_t* vol, addr_t addr, byte* data, size_t bytes)
{  
   dword sector = addr / vol->sb.sector_size;
   dword offset = addr % vol->sb.sector_size;
   
   byte* block = (byte*) malloc(512);
   floppy_read_block(sector, block, 512);
   memcpy(data, block+offset, bytes);
   free(block);
   
   return addr;
}

bool anica_write_dir(volume_t* vol, char* name, index_t parent)
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
 *    Searches for a space at least the size of the given
 *    size (in bytes), based on the entries in the index
 *    table. If a block is found, the address is returned.
 *    If not, a value of 0 is returned.
 *
 */
 
bool anica_is_addr_free(volume_t* vol, addr_t addr)
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
 
bool anica_block_fits(volume_t* vol, addr_t addr, size_t size)
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

addr_t anica_find_block(volume_t* vol, size_t size)
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






