#include "op.h"

void create_disk(int size, const char *name)
{
    FILE* disk;
    superblock block;
    char buff[BLOCK_SIZE];
    int i;
    
    block.number_of_blocks = ceil(size/4.0); // 16
    block.number_of_inodes = block.number_of_blocks; // 16
    block.inodeSize = sizeof(inode)+sizeof(short)*block.number_of_blocks; // 260 + 2*16 = 292
    block.inodeBitmapAddr = BLOCK_SIZE; //  4096
    block.dataBitmapAddr = 2*BLOCK_SIZE; // 2*4096
    block.inodeTableAddr = 3*BLOCK_SIZE; // 3*4096
    block.inodeSectorSize = BLOCK_SIZE*ceil(1.0*block.number_of_inodes*block.inodeSize/BLOCK_SIZE); //4096*(16*292/4096) = 292
    block.dataAddr = 3*BLOCK_SIZE + block.inodeSectorSize; // 3*4096 + 292 = 12580
    block.dataSectorSize = size*1024-block.dataAddr; // 64 * 1024 - 12580 = 52956
    block.freeInodes = block.number_of_inodes; // 16
    block.freeDataBlocks = block.number_of_blocks; // 16

    disk = fopen(name,"wb"); // create "disk"

    fill_buff(BLOCK_SIZE, buff, '0'); // fill the buff with 0
    
    //printf("sizeof(superblock) = %d\n",sizeof(superblock));

    disk_write(disk, sizeof(superblock), &block); // write superblock  to disk
    disk_write(disk, BLOCK_SIZE-sizeof(superblock), buff); // write buffer to disk 4kB

    disk_write(disk, BLOCK_SIZE, buff); //
    disk_write(disk, BLOCK_SIZE, buff); //
    for(i = 0; i < (block.inodeSectorSize+block.dataSectorSize)/BLOCK_SIZE; i++)
    {
        disk_write(disk, BLOCK_SIZE, buff); //

    }
    // DISK DONE!
    fclose(disk);
}

int main(int argc, char **argv)
{
    int size;
    if(argc != 3)
    {
        printf("Zle parametry wywolania. Sprobuj ./create [wielkosc](kB) [nazwa]\n");
        return 1;
    }
    size = atoi(argv[1]);
    create_disk(size,argv[2]);
    printf("Utworzono dysk o wielkosci %d i nazwie %s\n",size,argv[2]);
    return 0;
}
