#include "op.h"

void cpfromdisk(char * vdisk, char * filename, char * destination)
{
    FILE* disk = fopen(vdisk,"rb");
    short found = 0;
    int filesize;
    int i;
    int blocks_needed;
    FILE* dest;
    superblock sb;
    short * dataPointers;
    char * inodeBitmap;
    inode node;
    char buffer[BLOCK_SIZE];
    disk_read(disk,sizeof(superblock),&sb);
    dataPointers = (short *)malloc(sizeof(short)*sb.number_of_blocks);
    inodeBitmap = (char *)malloc(sb.number_of_blocks);
    disk_seek(disk,sb.inodeBitmapAddr);
    disk_read(disk,sb.number_of_blocks,inodeBitmap);
    for(i = 0; i < sb.number_of_inodes; i++)
    {
        if(inodeBitmap[i] == '1')
        {
            disk_seek(disk,sb.inodeTableAddr+i*sb.inodeSize);
            disk_read(disk,sizeof(inode),&node);
            if(strcmp(node.filename,filename) == 0)
            {
                found = 1;
                disk_read(disk,sb.number_of_blocks*sizeof(short),dataPointers);
                break;
            }
        }
    }
    if(!found)
        printf("Taki plik nie istnieje!\n");
    else
    {
        dest = fopen(destination,"wb");
        filesize = node.dataSize;
        blocks_needed = ceil(1.0*filesize/BLOCK_SIZE);
        for(i = 0; i < blocks_needed-1; i++)
        {
            disk_seek(disk,sb.dataAddr+dataPointers[i]*BLOCK_SIZE);
            disk_read(disk,BLOCK_SIZE,buffer);
            disk_write(dest,BLOCK_SIZE,buffer);
        }
        disk_seek(disk,sb.dataAddr+dataPointers[blocks_needed-1]*BLOCK_SIZE);
        disk_read(disk,filesize%BLOCK_SIZE,buffer);
        disk_write(dest,filesize%BLOCK_SIZE,buffer);
        fclose(dest);
    }
    fclose(disk);
}

int main(int argc, char ** argv)
{
    if(argc != 4)
    {
        printf("Zle parametry wywolania. Sprobuj ./copytodisk [nazwa_dysku] [nazwa_pliku] [sciezka_do_nowego_pliku]\n");
        return 1;
    }
    cpfromdisk(argv[1],argv[2],argv[3]);
    return 0;
}
