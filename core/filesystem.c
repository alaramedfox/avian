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

volume_t* mount(device_t device)
{
	volume_t* volume = new(volume_t);
	byte* scratch = (byte*) malloc(512);
	for(int i=0; i<512; i++) { scratch[i] = 0; }
	
	int i = DFS_GetVolInfo(device, scratch, 0, volume);
	
	free(scratch);
	
	if(i) { 
		print("Error mounting device\n");
		return NULL;
	}
	return volume;
}

file_t* open(volume_t* device, const char path[], byte mode)
{
	file_t* file = new(file_t);
	byte *scratch = (byte*) malloc(512);
	
	int i = DFS_OpenFile(device, (uint8_t*)path, mode, scratch, file);
	
	/* Delete the scratch buffer */
	free(scratch);
	
	if(i == 0 || i == 1) return file;
	else {
		print(fserr(i)); print("\n");
		return NULL;
	}
}

static size_t file_io(file_t* file, byte* buffer, size_t bytes, bool read)
{
	size_t bc = 0; // Number of bytes transferred
	byte* scratch = (byte*) malloc(512); // Allocate a scratch sector
	
	int result; // Result code (0 or 1 if OK, fail otherwise)
	
	if(read) result = DFS_ReadFile(file, scratch, buffer, &bc, bytes);
	else     result = DFS_WriteFile(file, scratch, buffer, &bc, bytes);
	
	free(scratch);
	
	if(result == 0 || result == 1) return bc;
	else {
		print(fserr(result)); print("\n");
		return 0;
	}
}

size_t read(file_t* file, byte* buffer, size_t bytes)
{
	return file_io(file, buffer, bytes, true);
}

size_t write(file_t* file, byte* buffer, size_t bytes)
{
	return file_io(file, buffer, bytes, false);
}

void close(file_t* file)
{
	free(file);
}

void unmount(volume_t *volume)
{
	free(volume);
}

char* fserr(int err)
{
	switch(err)
	{
		case DFS_OK: return "OK"; break;
		case DFS_EOF: return "End of file"; break;
		case DFS_WRITEPROT: return "File is write protected"; break;
		case DFS_NOTFOUND: return "File not found"; break;
		case DFS_PATHLEN: return "Path too long"; break;
		case DFS_DNOTFOUND: return "Directory not found"; break;
		case DFS_NOTDIR: return "Cannot open directory as a file"; break;
		
		default: return "General error"; break;
	}
}


uint32_t DFS_ReadSector(byte unit, byte *buffer, uint32_t sector, size_t count)
{
	//print("Reading from sector "); print(itoa(sector,DEC)); print("\n");
	if(unit == fda) {
		if(floppy_read_block(sector, buffer, count*512)) return 0;
		else return 1; // Failed to write block
	}
	else return 2; // Invalid device
}

uint32_t DFS_WriteSector(byte unit, byte *buffer, uint32_t sector, size_t count)
{
	//print("Writing to sector "); print(itoa(sector,DEC)); print("\n");
	if(unit == fda) {
		if(floppy_write_block(sector, buffer, count*512)) return 0;
		else return 1; // Failed to write block
	}
	else return 2; // Invalid device
}

/* End File Allocation Table driver */