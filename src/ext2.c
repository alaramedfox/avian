#define EXT2_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                             
//    File:          avian/ext2.c                                    
//    Purpose:       Implementation of the Second Extended Filesystem                    
// ======================================================================== */

#include <ext2.h>
#include <string.h>

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

int ext2_read_superblock(ext2_super_t* super)
{
   byte* buffer = (byte*) malloc(1024);
   ext2_read_sector(0, 2, buffer);
   ext2_read_sector(0, 3, buffer+512);
   memcpy(super, buffer, 1024);
   free(buffer);
   return 0;
}

ext2_table_t* ext2_read_table(ext2_super_t* super)
{
   size_t block_size = 1024 << super->block_size;
   
   size_t bsize = super->blocks_total*sizeof(ext2_group_t);
   size_t tsize = bsize/block_size;
   byte* buffer = (byte*) malloc(bsize);
   dword block_start = block_size==1024?2:1;
   
   foreach(i, tsize) {
      byte* temp_block = ext2_read_block(super, block_start+i);
      memcpy(buffer+(i*block_size), temp_block, block_size);
      free(temp_block);
   }
   
   ext2_table_t* block_table = new(ext2_table_t);
   block_table->table = (ext2_group_t*) malloc(bsize);
   memcpy(block_table->table, buffer, bsize);
   
   return block_table;
}

size_t ext2_block_groups(ext2_super_t* super)
{
   dword bpg = super->blocks_per_group;
   dword ipg = super->inodes_per_group;
   
   size_t by_blocks = super->blocks_total / (bpg + (bpg/2));
   size_t by_inodes = super->inodes_total / (ipg + (ipg/2));
   
   return by_blocks;
}

byte* ext2_read_block(ext2_super_t* super, dword index)
{
   size_t bs = 1024 << super->block_size;
   byte* buffer = (byte*) malloc(bs);
   size_t sectors = bs/512;
   foreach(i, sectors) {
      ext2_read_sector(0, (i*sectors) + index, buffer+(i*512));
   }
   return buffer;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //