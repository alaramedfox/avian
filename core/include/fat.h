#ifndef FAT_H_INCLUDED
#define FAT_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/fat.h
 *		Purpose:	Open, close, edit files on a FAT volume
 */
#include <types.h>
#include <mmap.h>
#include <dosfs.h>

typedef struct __FILE
{
	PVOLINFO volinfo;
	PDIRINFO dirinfo;
	PFILEINFO fileinfo;
	byte *buffer;

} file_t;

void open(file_t*, byte, const char[]);
size_t read(file_t*, size_t);
void close(file_t*);




#endif