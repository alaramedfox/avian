#define FAT_C_SOURCE
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/fat.c
 *		Purpose:	FAT filesystem driver
 */

#include <fat.h>
#include <floppy.h>
#include <stdlib.h>

enum __DEVICES
{
	fda, fdb, hda, hdb, hdc, hdd,
	sda, sdb, sdc, sdd,
};

enum __PARTITIONS
{
	fda0, fda1, fda2, fda3,
	fdb0, fdb1, fdb2, fdb3,
};

/*
 *		NOTE: Opening a file will take more than a KB of memory
 */
void open(file_t *file, byte mode, const char path[])
{
	/* Allocate a new file pointer and its fields */
	file = new(file_t);
	file->buffer = (byte*) malloc(512);
	file->volinfo = new(VOLINFO);
	file->dirinfo = new(DIRINFO);
	file->fileinfo = new(FILEINFO);
	
	/* Create a scratch space for temp data */
	byte* scratch = (byte*) malloc(512);
	
	addr_t start = DFS_GetPtnStart(fda, scratch, fda0, 0,0,0);
	DFS_GetVolInfo(fda, scratch, start, file->volinfo);
	DFS_OpenFile(file->volinfo, (uint8_t*)path, mode, scratch, file->fileinfo);
	
	/* Delete the scratch buffer */
	free(scratch);
}

size_t read(file_t *file, size_t size)
{
	/* Allocate temporary data */
	size_t *bc = new(size_t);
	byte* scratch = (byte*) malloc(512);
	
	DFS_ReadFile(file->fileinfo, scratch, file->buffer, bc, size);
	size_t bytes_read = *(bc);
	
	/* Delete temporary variables */
	free(bc);
	free(scratch);
	
	return bytes_read;
}

void close(file_t *file)
{
	free(file->buffer);
	free(file->volinfo);
	free(file->dirinfo);
	free(file->fileinfo);
	free(file);
}



uint32_t DFS_ReadSector(byte unit, byte *buffer, uint32_t sector, size_t count)
{
	if(unit == fda) {
		if(floppy_read_block(sector, buffer, count)) { return 0; }
		else return -1;
	}
	else return -1;
}

uint32_t DFS_WriteSector(byte unit, byte *buffer, uint32_t sector, size_t count)
{
	if(unit == fda) {
		if(floppy_write_block(sector, buffer, count)) { return 0; }
		else return -1;
	}
	else return -1;
}



/* End File Allocation Table driver */