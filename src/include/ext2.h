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
   EXT_OPT_PRE  = 0x01,   // Preallocate blocks
   EXT_OPT_AFS  = 0x02,   // AFS Server nodes exist
   EXT_OPT_JRNL = 0x04,  // Journaling exists (ext3)
   EXT_OPT_ATTR = 0x08,  // Inodes have attributes
   EXT_OPT_RESZ = 0x10,  // Automatic resize of partitions
   EXT_OPT_HASH = 0x20,  // Directories use hash tables
};

enum __EXT_REQ_FEATURES
{
   EXT_REQ_COMP = 0x01,  // Compression is used
   EXT_REQ_TYPE = 0x02,  // Directories contain a type field
   EXT_REQ_REPY = 0x04,  // Filesystem needs to handle its journal
   EXT_REQ_JRNL = 0x08,  // Filesystem uses a journaling device
};

enum __EXT_READONLY_FEATURES
{
   EXT_USP_SPAR = 0x01,  // Sparse tables are used
   EXT_USP_FS64 = 0x02,  // File sizes are 64 bits
   EXT_USP_TREE = 0x04,  // Filesystem uses a binary tree 
};

enum __EXT_INODE_TYPES
{
   EXT_INO_FIFO = 0x1000,
   EXT_INO_CHAR = 0x2000,
   EXT_INO_DIR  = 0x4000,
   EXT_INO_BLOC = 0x6000,
   EXT_INO_FILE = 0x8000,
   EXT_INO_LINK = 0xA000,
   EXT_INO_SOCK = 0xC000,
};

enum __EXT_INODE_FLAGS
{
   EXT_FLG_SDEL = 0x00000001,
   EXT_FLG_MDEL = 0x00000002,
   EXT_FLG_COMP = 0x00000004,
   EXT_FLG_SYNC = 0x00000008,
   EXT_FLG_IMMU = 0x00000010,
   EXT_FLG_APPD = 0x00000020,
   EXT_FLG_NINC = 0x00000040,
   EXT_FLG_LACC = 0x00000080,
   EXT_FLG_HASH = 0x00010000,
   EXT_FLG_AFS  = 0x00020000,
   EXT_FLG_JRNL = 0x00040000,
};

enum __EXT_DIR_TYPES
{
   EXT_DIR_UNKN = 0,
   EXT_DIR_FILE = 1,
   EXT_DIR_DIR  = 2,
   EXT_DIR_CHAR = 3,
   EXT_DIR_BLOC = 4,
   EXT_DIR_FIFO = 5,
   EXT_DIR_SOCK = 6,
   EXT_DIR_LINK = 7,
};

typedef struct __EXT_SUPER
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
   
   /* These byts are not used (I think), but need to exist as filler */
   byte zero[788];

} packed ext2_super_t;

typedef struct __EXT_GROUP
{
   dword block_bitmap_addr;
   dword inode_bitmap_addr;
   dword inode_table_start;
   word blocks_free;
   word inodes_free;
   word dirs;
   byte zero[17];

} packed ext2_group_t;

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

} packed ext2_inode_t;

typedef struct __EXT_DIRECTORY
{
   dword inode;
   word size;
   byte name_size;
   byte type;
   char name[24];

} packed ext2_dir_t;

typedef struct __EXT_CACHE
{
   ext2_group_t* group_table;
   ext2_inode_t* inode_table;
   byte* block_bitmap;
   byte* inode_bitmap;
   
} packed ext2_cache_t;


// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

/* Functions that must be defined by user */
int    ext2_read_sector(byte device, dword sector, byte* buffer);
int    ext2_write_sector(byte device, dword sector, byte* buffer);

/* Ext2 functions */
int    ext2_read_superblock(ext2_super_t* block);
size_t ext2_block_groups(ext2_super_t* block);

ext2_cache_t* ext2_read_cache(ext2_super_t* super, dword group);
byte*  ext2_read_block(ext2_super_t* super, dword index);

#endif