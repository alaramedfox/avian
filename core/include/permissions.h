#ifndef PERMISSIONS_H_INCLUDED
#define PERMISSIONS_H_INCLUDED
/*
 *		Avian Kernel - Bryan Webb
 *		File:		permissions.h
 *		Purpose:	Define bitfield that handles permissions
 */
 
#include <types.h>

enum __LEVELS
{
	/*Kernel, Captain, User, Anyone */
	KNL=0, CAP=1, USR=2, ALL=3,  
};

typedef struct __PERMIT
{
	byte view 	: 2;	//Visibility of object
	byte read 	: 2;	//Ability to read data
	byte write 	: 2;	//Ability to write data
	byte alloc  : 2;  //Ability to modify structure
	
} permit_t;


#endif