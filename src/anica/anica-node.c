#define ANICA_NODE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                              
//    File:          avian/anica/anica-node.c                                   
//    Purpose:       AnicaFS Object nodes                    
// ======================================================================== */

#include <anicafs.h>
#include <stdlib.h>
#include <string.h>

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

anode_t anica_make_file(volume_t* vol, char* path, size_t size)
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

addr_t anica_read_node(volume_t* vol, aentry_t* entry, anode_t* node)
{  
   return anica_read_data(vol, entry->addr, (byte*)node, sizeof(anode_t));
}

addr_t anica_write_node(volume_t* vol, aentry_t* entry, anode_t* node)
{
   return anica_write_data(vol, entry->addr, (byte*)node, sizeof(anode_t));
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //