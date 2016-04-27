#define DEVICE_C_SOURCE
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/device/device.c                                    
//    Purpose:       Main device interface                    
// ======================================================================== */

#include <stdlib.h>
#include <string.h>
#include <device.h>
#include <floppy.h>
#include <stdio.h>

#define USE_DEVICE_CACHE 1

// ========================================================================= //
//       Private variables and function prototypes                           //
// ========================================================================= //

static int device_io(device_t*, dword, byte*, bool);
static int device_cache_index(device_t*, dword);
static int device_add_cache(device_t*, dword, byte*);

static char* device_string_list[] = {
   "fda", "fdb", "fdc", "fdd",
   "hda", "hdb", "hdc", "hdd",
   "sda", "sdb", "sdc", "sdd",
   "NO DEVICE"
};

// ========================================================================= //
//       Public API Implementation                                           //
// ========================================================================= //

int mount(byte id, device_t* dev)
{
   if(id == fda) {
      dev->id = fda;
      dev->sector_size = 512;
      dev->device_size = 2880;
      dev->read_sector = floppy_read_block;
      dev->write_sector = floppy_write_block;
   }
   else {
      dev = NULL;
      return DEV_NODEV;
   }
   
   dev->cache.size = 0;
   dev->cache.max = 8;
   dev->cache.entry = (dc_entry_t*) malloc(8);
   return DEV_OKAY;
}

int unmount(device_t* dev)
{
   sync(dev);
   foreach(i, dev->cache.size) {
      free(dev->cache.entry[i].data);
   }
   free(dev->cache.entry);
   return DEV_OKAY;
}

int sync(device_t* dev)
{
   foreach(i, dev->cache.size) {
      if(dev->cache.entry[i].changed) {
         printf("Syncing sector %i\n", dev->cache.entry[i].sector);
         dword addr = dev->cache.entry[i].sector;
         byte* data = dev->cache.entry[i].data;
         if(device_io(dev, addr, data, false)) {
            return DEV_IOERR;
         }
      }
   }
   return DEV_OKAY;
}

int read(device_t* dev, dword addr, byte* data)
{
#if USE_DEVICE_CACHE
   int index = device_cache_index(dev, addr);
   if(index != -1) {
      //printf("Reading from cached sector %i\n", addr);
      byte* cache = dev->cache.entry[index].data;
      size_t size = dev->sector_size;
      memcpy(data, cache, size);
      return DEV_OKAY;
   }
   else {
      device_io(dev, addr, data, true);
      device_add_cache(dev, addr, data);
      return DEV_OKAY;
   }
#else
   return device_io(dev, addr, data, true);
#endif
}

int write(device_t* dev, dword addr, byte* data)
{
#if USE_DEVICE_CACHE
   int index = device_cache_index(dev, addr);
   if(index != -1) {
      //printf("Writing to cached sector %i\n", addr);
      size_t size = dev->sector_size;
      memcpy(dev->cache.entry[index].data, data, size);
      dev->cache.entry[index].changed = true;
      return DEV_OKAY;
   }
   else {
      device_add_cache(dev, addr, data);
      //device_io(dev, addr, data, false);
      return DEV_OKAY;
   }
#else
   return device_io(dev, addr, data, false);
#endif
}

char* devtoa(byte id)
{
   if(id < NO_DEV) {
      return device_string_list[id];
   }
   else {
      return "NO DEVICE";
   }
}

byte atodev(char* str)
{
   foreach(i, NO_DEV) {
      if(strcmp(str, device_string_list[i]) == 0) {
         return i;
      }
   }
   return NO_DEV;
}

// ========================================================================= //
//       Private functions                                                   //
// ========================================================================= //

static int device_io(device_t* dev, dword addr, byte* data, bool read)
{
   if(read) {
      return dev->read_sector(addr, data);
   }
   else {
      return dev->write_sector(addr, data);
   }
}

static int device_add_cache(device_t* dev, dword sector, byte* data)
{
   //printf("Adding sector %i to cache\n", sector);
   /* Check if the cache needs to expand */
   index_t index = dev->cache.size;
   if(index >= dev->cache.max) {
      printf("Expanding cache\n");
      dev->cache.max += 8;
      dev->cache.entry = (dc_entry_t*) realloc(dev->cache.entry, dev->cache.max);
   }
   size_t size = dev->sector_size;
   dev->cache.entry[index].sector = sector;
   dev->cache.entry[index].changed = false;
   dev->cache.entry[index].data = (byte*) malloc(size);
   memcpy(dev->cache.entry[index].data, data, size);
   dev->cache.size++;
   
   return DEV_OKAY;
}

static int device_cache_index(device_t* dev, dword sector)
{
   foreach(i, dev->cache.size) {
      if(dev->cache.entry[i].sector == sector) {
         return i;
      }
   }
   return -1;
}






