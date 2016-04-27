#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED
// ======================================================================== */
//    Avian Kernel   Bryan Webb (C) 2016                               
//    File:          avian/include/device.h                                   
//    Purpose:       Device Library                    
// ======================================================================== */

#include <stdlib.h>
#include <stdio.h>

/**
 *    Avian_Documentation:
 *    Enumeration of the possible device types.
 *    These should be mapped to the BIOS devices.
 */
enum __DEVICE_ID
{
   fda=0, fdb, hda, hdb, hdc, hdd,
   sda, sdb, sdc, sdd,
   
   NO_DEV,
};

/**
 *    Avian_Documentation:
 *    Enumeration of the error codes returned by device library
 *    functions. Functions will return 0 upon success, and a
 *    nonzero value upon failure. The exact value of the return
 *    corresponds to the error code (if any)
 */
enum __DEVICE_RETURN
{
   DEV_OKAY  = 0, // Function completed successfully
   DEV_NODEV = 1, // Device not found or could not be accessed
   DEV_IOERR = 2, // I/O function failed
   
   DEV_ERR,       // General failure
};

/**
 *    Avian_Documentation:
 *    Structure for device data caches. Instead of constantly
 *    reading and writing to the same blocks on a device, the
 *    reads and writes are done in memory until sync() is called.
 */
typedef struct __DEVICE_CACHE_ENTRY
{
   dword sector;  // Sector number of cached data
   bool changed;  // Flag for if the data needs to be updated on disk
   byte* data;    // Pointer to a sector-sized array of bytes

} dc_entry_t;

typedef struct __DEVICE_CACHE_LIST
{
   size_t size;         // Current number of entries
   size_t max;          // Maximum number of entries before expansion
   dc_entry_t* entry;   // Pointer to array of cache entries

} devcache_t;

/**
 *    Avian_Documentation:
 *    Structure for a device object. When an object is mounted,
 *    this structure abstracts the device driver's IO functions
 *    to higher-level IO functions. 
 *
 *    The structure contains the BIOS drive ID, the device geometry,
 *    pointers to the device's IO functions, and a pointer 
 *    to the device cache.
 */
typedef struct __DEVICE
{
   byte id;             // BIOS Device ID number
   size_t sector_size;  // Size of sectors in bytes
   size_t device_size;  // Total size of device in sectors
   devcache_t cache;    // Device cache
   
   int (*read_sector)(dword lba, byte* buffer);
   int (*write_sector)(dword lba, byte* buffer);
   
} device_t;

// ========================================================================= //
//       Public API Prototypes                                               //
// ========================================================================= //

int mount(byte id, device_t* dev);
int unmount(device_t* dev);
int sync(device_t* dev);
int read(device_t* dev, dword addr, byte* data);
int write(device_t* dev, dword addr, byte* data);

char* devtoa(byte id);
byte  atodev(char*);

#endif