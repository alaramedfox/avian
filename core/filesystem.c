#define FAT_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/filesystem.c
 *		Purpose:	Filesystem abstraction interface
 */

#include <filesystem.h>
#include <floppy.h>
#include <stdlib.h>
#include <util.h>
#include <vga.h>



enum __PARTITIONS
{
	fda0, fda1, fda2, fda3,
	fdb0, fdb1, fdb2, fdb3,
};

/*
 *		NOTE: Opening a file will take more than a KB of memory
 */

fatvol_t* mount(device_t device)
{
	fatvol_t* volume = new(fatvol_t);
	byte* buffer = (byte*) malloc(512);
	
	DFS_GetVolInfo(0, buffer, 0, volume);
	
	free(buffer);
	return volume;
}

file_t* open(volume_t* device, const char path[], byte mode)
{
	file_t* file = new(file_t);
	byte* buffer = (byte*) malloc(512);
	
	DFS_OpenFile(device, path, mode, buffer, file);
	
	free(buffer);
	return file;
}

static size_t file_io(file_t* file, byte* buffer, size_t bytes, bool read)
{
	size_t bc = 0; // Number of bytes transferred
	
	return bc;
}

size_t read(file_t* file, byte* buffer, size_t bytes)
{
	return file_io(file, buffer, bytes, true);
}

size_t write(file_t* file, byte* buffer, size_t bytes)
{
	return file_io(file, buffer, bytes, false);
}

void close(FILEINFO* file)
{
	free(file);
}

void unmount(VOLINFO* volume)
{
	free(volume);
}

char* fserr(int err)
{
	return "?";
}

uint32_t DFS_ReadSector(uint8_t unit, uint8_t *buffer, uint32_t sector, uint32_t count)
{
   return !floppy_read_block(sector, buffer, count*512);
}
uint32_t DFS_WriteSector(uint8_t unit, uint8_t *buffer, uint32_t sector, uint32_t count)
{
   return !floppy_write_block(sector, buffer, count*512);
}












/* End File Allocation Table driver */