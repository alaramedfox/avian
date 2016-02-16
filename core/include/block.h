#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		/core/include/block.h
 *		Purpose:	Header for the block datatype
 */
 
#include <types.h>

/* Blocks are chuncks of 512 bytes */

typedef struct __BLOCK
{
	byte 		id;			//Information about the block itself
	addr_t   source;		//Source address
	byte 		data[512];	//Block of actual data
	
} block_t;
#endif