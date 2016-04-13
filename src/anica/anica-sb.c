#define ANICA_SB_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                            
//    File:          avian/anica/anica-sb.c                                   
//    Purpose:       Source for the Anica Superblock                    
// ======================================================================== */

#include <anicafs.h>
#include <stdlib.h>
#include <string.h>
#include <floppy.h>

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

bool anica_read_superblock(byte device, asuper_t* superblock)
{  
   word sb_sector = anica_find_superblock();

   byte* block = (byte*) calloc(512, 1);
   if(device == 0) floppy_read_block(sb_sector, block, 512);
   memcpy(superblock, block, sizeof(asuper_t));
   superblock->hidden = sb_sector;
   
   free(block);
   return true;
}

bool anica_write_superblock(byte device, asuper_t* superblock)
{  
   byte* block = (byte*) calloc(512, 1);
   word sb_sector = anica_find_superblock();
   //if(device == 0) floppy_read_block(superblock->hidden, block, 512);
   if(device == 0) floppy_read_block(sb_sector, block, 512);
   memcpy(block, superblock, sizeof(asuper_t));
   //if(device == 0) floppy_write_block(superblock->hidden, block, 512);
   if(device == 0) floppy_write_block(sb_sector, block, 512);
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

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //