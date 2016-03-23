#ifndef EXT2_H_INCLUDED
#define EXT2_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                             
//    File:          avian/include/ext2.h                                   
//    Purpose:       Implementation of the Second Extended Filesystem                    
// ======================================================================== */

#include <stdlib.h>

enum __EXT_STATES
{
   EXT_CLEAN = 1, EXT_ERROR = 2,
};

enum __EXT_ACTIONS
{
   EXT_IGNORE = 1, EXT_REMOUNT = 2, EXT_PANIC = 3,
};

enum __EXT_OS_IDS
{
   EXT_LINUX, EXT_HURD, EXT_MASIX, EXT_BSD, EXT_MISC,
};

enum __EXT_OPT_FEATURES
{
   EXT_PRE = 0x01,   // Preallocate blocks
   EXT_AFS = 0x02,   // AFS Server nodes exist
   EXT_JRNL = 0x04,  // Journaling exists (ext3)
   EXT_ATTR = 0x08,  // Inodes have attributes
   EXT_RESZ = 0x10,  // Automatic resize of partitions
   EXT_HASH = 0x20,  // Directories use hash tables
};

enum __EXT_REQ_FEATURES
{
   EXT_COMP = 0x01,  // Compression is used
   EXT_TYPE = 0x02,  // Directories contain a type field
   EXT_REPY = 0x04,  // Filesystem needs to handle its journal
   EXT_JDEV = 0x08,  // Filesystem uses a journaling device
};

enum __EXT_READONLY_FEATURES
{
   EXT_SPAR = 0x01,  // Sparse tables are used
   EXT_FS64 = 0x02,  // File sizes are 64 bits
   EXT_TREE = 0x04,  // Filesystem uses a binary tree 
};

enum __EXT_INODE_TYPES
{
   EXT_FIFO_T = 0x1000,
   EXT_CDEV_T = 0x2000,
   EXT_DIR_T  = 0x4000,
   EXT_BDEV_T = 0x6000,
   EXT_FILE_T = 0x8000,
   EXT_LINK_T = 0xA000,
   EXT_SKET_T = 0xC000,
};

enum __EXT_INODE_FLAGS
{
   EXT_SDEL_F = 0x00000001,
   EXT_MDEL_F = 0x00000002,
   EXT_COMP_F = 0x00000004,
   EXT_SYNC_F = 0x00000008,
   EXT_IMMU_F = 0x00000010,
   EXT_APPD_F = 0x00000020,
   EXT_NINC_F = 0x00000040,
   EXT_LACC_F = 0x00000080,
   EXT_HASH_F = 0x00010000,
   EXT_AFSD_F = 0x00020000,
   EXT_JDAT_F = 0x00040000,
};

typedef struct __EXT_SUPERBLOCK
{
   dword inodes_total;
   dword blocks_total;
   dword reserved;
   dword blocks_free;
   dword inodes_free;
   dword superblock;
   dword block_size;    // Size = (1024 << X)
   dword frag_size;     // Size = (1024 << X)
   dword blocks_per_group;
   dword frags_per_group;
   dword inodes_per_group;
   dword mount_time;    // Not implemented
   dword write_time;    // Not implemented
   word mount_count;
   word mount_max;
   word ext2_sig;       // 0xEF53
   word ext2_state;
   word error_action;
   word version_minor;
   dword check_last;    // Not implemented
   dword check_max;     // Not implemented
   dword os_id;
   dword version_major;
   word user_id;
   word group_id;
   
   /* These fields are present if version is >= 1 */
   dword first_inode;
   word inode_size;
   word sb_group;
   dword opt_features;  // Implemented optional features
   dword req_features;  // Implemented required features
   dword usp_features;  // Unsupported features
   char oemid[16];      // Filesystem ID
   char volid[16];      // Volume label
   char point[64];      // C String of last mount point
   dword calgs;         // Compression algorithms used
   byte f_prealloc;     // Number of blocks to preallocate for files
   byte d_prealloc;     // Number of blocks to preallocate for dirs
   word unused;
   char journal_id[16];      // Same as oemid
   dword journal_inode; 
   dword journal_dev;
   dword orphan_list;   // Head of orphan inode list

} FLAT ext2_sb_t;

typedef struct __EXT_GROUP_DESC
{
   dword block_bitmap_addr;
   dword inode_bitmap_addr;
   dword inode_table_start;
   word blocks_free;
   word inodes_free;
   word dirs;
   byte zero[17];

} FLAT ext2_group_t;

typedef struct __EXT_INODE
{
   word info;
   word uid;
   dword size_lower;
   dword timestamps[4];
   word gid;
   word hard_links;
   dword size_sectors;
   dword flags;
   dword os_value_1;
   dword direct_pointers[12];
   dword indirect_pointers[3];
   dword genid;
   dword file_acl;
   dword size_upper;
   dword frag_addr;
   dword os_value_2;
   dword os_value_3;
   dword os_value_4;

} FLAT ext2_inode_t;


// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

size_t ext2_block_groups(ext2_sb_t* superblock);

#endif