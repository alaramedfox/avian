#define ANICA_TABLE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/anica/anica-table.c                                    
//    Purpose:       AnicaFS Index Table                    
// ======================================================================== */

#include <anicafs.h>
#include <stdlib.h>
#include <string.h>
#include <floppy.h>

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

/**
 *    Avian_Documentation:    
 *    Reads/writes the given itable from the volume into the provided
 *    table pointer. If the table is read correctly, then the 
 *    function returns the number of elements in the table. If
 *    the function cannot read the table, a value of -1 is
 *    returned. 
 */

int anica_read_itable(device_t* device, volume_t* vol)
{  
   int sector = vol->sb.table_addr;
   size_t tsize = vol->sb.table_size * sizeof(aentry_t);
   
   size_t sectors = tsize / vol->sb.sector_size;
   if(tsize % vol->sb.sector_size > 0) sectors++;
   
   byte* block = (byte*) malloc(tsize);
   
   foreach(i, sectors) {
      read(device, sector+i, block+(i*vol->sb.sector_size));
   }
   memcpy(vol->itable, block, tsize);
   free(block);
   return ANICA_OK;
}

int anica_write_itable(device_t* device, volume_t* vol)
{  
   int sector = vol->sb.table_addr;
   size_t tsize = vol->sb.table_size * sizeof(aentry_t);
   
   size_t sectors = tsize / vol->sb.sector_size;
   if(tsize % vol->sb.sector_size > 0) sectors++;
   
   byte* block = (byte*) malloc(tsize);
   memcpy(block, vol->itable, tsize);
   
   foreach(i, sectors) {
      write(device, sector+i, block+(i*vol->sb.sector_size));
   }
   free(block);
   return ANICA_OK;
}

int anica_parent_index(volume_t* vol, char* path, char* filename)
{
   char** tree = (char**) malloc(64);
   size_t depth = split('/',0,path,tree);
   
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

aentry_t anica_add_entry(volume_t* vol, byte type, index_t parent, size_t size)
{
   aentry_t entry;
   entry.type = type;
   entry.size = size;
   entry.parent = parent;
   entry.addr = anica_find_block(vol, entry.size);
   vol->itable[vol->sb.entries++] = entry;
   return entry;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //