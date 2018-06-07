/*
	Podstawowe operacje
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>

#define BLOCK_SIZE 4096
#define MAX_FILENAME 256

typedef struct superblock
{
    
//    inode bitmap - indicates which inodes are allocated
//    inodes - table of inodes and their contents
//    data bitmap - indicates which data blocks are allocated
//    data - indicates contents of data blocks

	int number_of_blocks;
	int number_of_inodes;
	int inodeTableAddr;
	int inodeBitmapAddr;
	int dataBitmapAddr;
	int dataAddr;
	int inodeSize;
	int inodeSectorSize;
	int dataSectorSize;
	int freeInodes;
	int freeDataBlocks;
} superblock;

typedef struct inode
{
    char filename[MAX_FILENAME];
    int dataSize;
} inode;

void disk_seek(FILE* disk, int offset);
void disk_read(FILE* disk, int size, void* buff);
void disk_write(FILE* disk, int size, void *buff);
void fill_buff(int size, char *name, char character);
