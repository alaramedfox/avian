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
	
	return volume;
}

file_t* open(volume_t* device, const char path[], byte mode)
{
	file_t* file = new(file_t);
	
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
	return "?";
}

/* End File Allocation Table driver */