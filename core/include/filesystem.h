#ifndef FAT_H_INCLUDED
#define FAT_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/filesystem.h
 *		Purpose:	Open, close, edit files on a given volume
 */
#include <types.h>
#include <mmap.h>
#include <dosfs.h>

typedef enum __DEVICES
{
	fda=0, fdb, hda, hdb, hdc, hdd,
	sda, sdb, sdc, sdd,
	
} device_t;

typedef FILEINFO file_t;
typedef VOLINFO volume_t;

volume_t* 	mount(device_t);
void			unmount(volume_t*);
file_t* 		open(volume_t*, const char[], byte);
size_t 		read(file_t*, byte*, size_t);
size_t 		write(file_t*, byte*, size_t);
void 			close(file_t*);
char* 		fserr(int);



#endif