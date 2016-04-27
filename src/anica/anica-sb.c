#define ANICA_SB_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                            
//    File:          avian/anica/anica-sb.c                                   
//    Purpose:       Source for the Anica Superblock                    
// ======================================================================== */

#include <anicafs.h>
#include <stdlib.h>
#include <string.h>
#include <device.h>

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

bool anica_read_superblock(filesystem_t* fs)
{  
   volume_t* vol = (volume_t*) fs->master;
   
   word sb_sector = anica_find_superblock();

   byte* block = (byte*) calloc(512, 1);
   read(fs->device, sb_sector, block);
   memcpy(&vol->sb, block, sizeof(asuper_t));
   vol->sb.hidden = sb_sector;
   
   free(block);
   return true;
}

bool anica_write_superblock(filesystem_t* fs)
{  
   volume_t* vol = (volume_t*) fs->master;
   
   word sb_sector = anica_find_superblock();

   byte* block = (byte*) calloc(512, 1);
  
   memcpy(block, &vol->sb, sizeof(asuper_t));
   write(fs->device, sb_sector, block);
   
   free(block);
   return true;
}

void anica_format_sb(asuper_t* superblock, size_t sec, size_t bps, size_t res)
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

word anica_find_superblock(void)
{  
   return 0;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //