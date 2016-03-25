#define EXT2_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                             
//    File:          avian/ext2.c                                    
//    Purpose:       Implementation of the Second Extended Filesystem                    
// ======================================================================== */

#include <ext2.h>
#include <string.h>
#include <stdio.h>

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

ext2_cache_t* ext2_read_cache(ext2_super_t* super, dword group)
{
   ext2_cache_t* cache = new(ext2_cache_t);
   size_t block_size = 1024 << super->block_size;
   
   /* Read the Block Group Table */
   size_t entries = ext2_block_groups(super); 
   size_t size = entries * sizeof(ext2_group_t);
   size_t blocks = size/block_size + (size%block_size?1:0);
   byte* buffer = (byte*) malloc(size); 
   dword start = block_size==1024?2:1;
   
   foreach(i, blocks) {
      byte* temp = ext2_read_block(super, start+i);
      memcpy(buffer+(i*block_size), temp, block_size);
      free(temp);
   }
   
   cache->group_table = (ext2_group_t*) malloc(entries*sizeof(ext2_group_t));
   memcpy(cache->group_table, buffer, size);
   free(buffer);
   
   /* Read the Inode Table for the given group */
   start = cache->group_table[group].inode_table_start;
   entries = super->inodes_per_group;
   printf("Entries: %i\n",entries);
   size = entries * sizeof(ext2_inode_t);
   blocks = size/block_size + (size%block_size?1:0);
   cache->inode_table = (ext2_inode_t*) malloc(size);
   printf("Size: %i\n",size);
   buffer = (byte*) malloc(size);
   
   foreach(i, blocks) {
      byte* temp = ext2_read_block(super, start+i);
      memcpy(buffer+(i*block_size), temp, block_size);
      free(temp);
   }
   
   memcpy(cache->inode_table, buffer, size);
   free(buffer);
   
   return cache;
}

size_t ext2_block_groups(ext2_super_t* super)
{
   dword bpg = super->blocks_per_group;
   dword ipg = super->inodes_per_group;
   dword bt = super->blocks_total;
   dword it = super->inodes_total;
   
   size_t by_blocks = bt/bpg + bt%bpg?1:0;
   size_t by_inodes = it/ipg + it%ipg?1:0;
   
   return by_blocks?by_inodes:by_blocks;
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