#define ANICAFS_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                                  
//    File:          avian/anicafs.c                                     
//    Purpose:       Implementation of the Linda FS                      
// ======================================================================== */

#include <anicafs.h>

#include <string.h>
#include <errors.h>
#include <stdio.h>

// ======================================================================== //
//           Public API functions                                           //
// ======================================================================== //

/**
 *    Avian_Documentation:
 *    Reads an AnicaFS filesystem off the provided device into the
 *    provided empty volume structure. It reads the superblock and
 *    index table and everything else needed to use all the AnicaFS
 *    functions. If the volume is read successfully, the function
 *    returns 0. If not, a nonzero value indicating the error is
 *    returned.
 */
int anica_read_device(filesystem_t* fs)
{
   anica_read_superblock(fs);
   anica_read_itable(fs);
   
   return 0;
}

bool anica_format_device(filesystem_t* fs, size_t sec, size_t bps, size_t res)
{  
   /* Format the superblock */
   volume_t* vol = (volume_t*) fs->master;
   anica_format_sb(&vol->sb, sec, bps, res);
   vol->itable = (aentry_t*) malloc(vol->sb.table_size * sizeof(aentry_t));
   
   /* Create directory structure */
   anica_write_dir(fs, "$", 0); // Root
   anica_mkdir(fs,"$/sys");
   
   anica_mkdir(fs,"$/lib");
   anica_mkdir(fs,"$/app");
   anica_mkdir(fs,"$/usr");
   
   char grub_config[] = "title Avian Kernel\nkernel 2680+160\nboot";
   
   /* Reserve file blocks for GRUB and the kernel */
   size_t kernel_size = 96 * 1024;  // 96 KiB
   size_t stage2_size = 102 * 1024; // 102 KiB
   size_t config_size = strlen(grub_config)+5;
   
   anode_t kernel = anica_make_file(fs, "$/sys/avian.bin", kernel_size);
   //anode_t stage2 = anica_make_file(vol, "$/sys/grub/stage2", stage2_size);
   //anode_t grubcf = anica_make_file(vol, "$/sys/grub/menu.lst", config_size);
   
   //vol->itable[grubcf.data].addr = 0x18E00;
   
   //anica_write_file(vol, (byte*)grub_config, &grubcf);
   
   
   vol->itable[kernel.data].addr = 1372160;  // Sector 2680
   //vol->itable[stage2.data].addr = 0x200;    // Sector 1
   
   
   anica_write_superblock(fs);
   anica_write_itable(fs);
   //free(vol->itable);
   //free(vol);
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
int anica_list_contents(filesystem_t* fs, const char path[], char** list)
{
   if(fs == NULL) return -1;
   volume_t* vol = (volume_t*) fs->master;
   
   anode_t node;
   int index = anica_read_path(fs, path, &node);
   if(index < 0) return index;
   
   size_t entries = 0;
   list[0] = (char*) malloc(3);
   list[1] = (char*) malloc(3);
   strcpy(list[entries++], "@/");
   strcpy(list[entries++], "^/");
   foreach(i, vol->sb.entries) {
      if(vol->itable[i].type != ANICA_DATA && vol->itable[i].parent == index) {
         anica_read_node(fs, &vol->itable[i], &node);
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

int anica_open_file(filesystem_t* fs, const char path[], byte mode, anode_t* file)
{  
   int status;
   anode_t node;
   int index = anica_read_path(fs, path, &node);
   
   if(index >= 0) {
      *(file) = node;
      status = ANICA_OK;
   }
   else if(mode == ANICA_WRITE) {
      node = anica_make_file(fs, path, ANICA_MIN_SIZE);
      *(file) = node;
      status = ANICA_OK;
   }
   else {
      file = NULL;
      status = ANICA_NOFILE;
   }

   return status;
}

int anica_read_file(filesystem_t* fs, byte* data, anode_t* node)
{  
   volume_t* vol = (volume_t*) fs->master;
   
   size_t bytes = vol->itable[node->data].size;
   dword address = vol->itable[node->data].addr;
   byte* buffer = (byte*) malloc(bytes);
   anica_read_data(fs, address, buffer, bytes);
   
   for(int i=0; buffer[i] != EOF; i++) {
      data[i] = buffer[i];
   }
   
   free(buffer);
   return 0;
}

int anica_write_file(filesystem_t* fs, byte* data, anode_t* node)
{  
   volume_t* vol = (volume_t*) fs->master;
   
   size_t bytes = vol->itable[node->data].size;
   addr_t address = vol->itable[node->data].addr;
   
   size_t datasize = ptrsize(data);
   /* If the file will be larger, re-allocate the file */
   if(datasize > bytes) {
      address = anica_find_block(fs, datasize);
      bytes = datasize;
      vol->itable[node->data].size = bytes;
      vol->itable[node->data].addr = address;
      anica_write_node(fs, &vol->itable[node->data], node);
   }
   data[bytes-1] = EOF;
   anica_write_data(fs, address, data, bytes);
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
int anica_read_path(filesystem_t* fs, const char path[], anode_t* node)
{
   volume_t* vol = (volume_t*) fs->master;
   
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
            anica_read_node(fs, &entry, node);
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

addr_t anica_write_data(filesystem_t* fs, addr_t addr, byte* data, size_t bytes)
{  
   volume_t* vol = (volume_t*) fs->master;
   
   dword sector = addr / vol->sb.sector_size;
   dword offset = addr % vol->sb.sector_size;
   
   byte* block = (byte*) malloc(512);
   read(fs->device, sector, block);
   memcpy(block+offset, data, bytes);
   write(fs->device, sector, block);
   free(block);
   
   return addr;
}

addr_t anica_read_data(filesystem_t* fs, addr_t addr, byte* data, size_t bytes)
{  
   volume_t* vol = (volume_t*) fs->master;
   
   dword sector = addr / vol->sb.sector_size;
   dword offset = addr % vol->sb.sector_size;
   
   byte* block = (byte*) malloc(512);
   read(fs->device, sector, block);
   memcpy(data, block+offset, bytes);
   free(block);
   
   return addr;
}

bool anica_write_dir(filesystem_t* fs, char* name, index_t parent)
{  
   volume_t* vol = (volume_t*) fs->master;
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
   aentry_t entry = anica_add_entry(fs, ANICA_DIR, parent, sizeof(anode_t));
   anica_write_node(fs, &entry, &dir);
   
   return status;
   
}



/**
 *    Searches for a space at least the size of the given
 *    size (in bytes), based on the entries in the index
 *    table. If a block is found, the address is returned.
 *    If not, a value of 0 is returned.
 *
 */
 
bool anica_is_addr_free(filesystem_t* fs, addr_t addr)
{  
   volume_t* vol = (volume_t*) fs->master;
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
 
bool anica_block_fits(filesystem_t* fs, addr_t addr, size_t size)
{  
   volume_t* vol = (volume_t*) fs->master;
   size_t bs = 0;
   addr_t max = vol->sb.volume_size * vol->sb.sector_size;
   
   while(anica_is_addr_free(fs, addr+bs) && (addr < max))
   {
      ++bs;
      if(bs >= size) return true;
   }
   return false;
}

addr_t anica_find_block(filesystem_t* fs, size_t size)
{  
   volume_t* vol = (volume_t*) fs->master;
   
   addr_t search_start = (512*vol->sb.table_addr + vol->sb.table_size*sizeof(aentry_t));
   addr_t max = vol->sb.volume_size * vol->sb.sector_size - vol->sb.sector_size;
   
   for(addr_t address = search_start; address < max; address+=size)
   {
      if(anica_block_fits(fs, address, size)) {
         return address;
      }
      else continue;
   }

   return 0;
}






