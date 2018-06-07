#include "op.h"

void rm(char * vdisk, char * filename)
{
    FILE* disk = fopen(vdisk,"r+b");
    int i;
    superblock sb;
    short *dataPointers;
    char *inodeBitmap;
    char *dataBitmap;
    int inodePointer;
    inode node;
    short found;
    disk_read(disk,sizeof(superblock),&sb);
    dataPointers = (short *)malloc(sizeof(short)*sb.number_of_blocks);
    inodeBitmap = (char *)malloc(sb.number_of_blocks);
    dataBitmap = (char *)malloc(sb.number_of_blocks);
    found = 0;
    disk_seek(disk,sb.inodeBitmapAddr);
    disk_read(disk,sb.number_of_blocks,inodeBitmap);
    disk_seek(disk,sb.dataBitmapAddr);
    disk_read(disk,sb.number_of_blocks,dataBitmap);
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
                inodePointer = i;
                break;
            }
        }
    }
    if(!found)
        printf("Taki plik nie istnieje!\n");
    else
    {
        inodeBitmap[inodePointer] = '0';
        for(i = 0; i < ceil(1.0*node.dataSize/BLOCK_SIZE);i++)
            dataBitmap[dataPointers[i]] = '0';
        disk_seek(disk,sb.inodeBitmapAddr);
        disk_write(disk,sb.number_of_blocks,inodeBitmap);
        disk_seek(disk,sb.dataBitmapAddr);
        disk_write(disk,sb.number_of_blocks,dataBitmap);
        sb.freeInodes++;
        sb.freeDataBlocks += i;
        disk_seek(disk,0);
        disk_write(disk,sizeof(superblock),&sb);
    }
    fclose(disk);
    free(dataPointers);
    free(inodeBitmap);
    free(dataBitmap);
}

int main(int argc, char ** argv)
{
    if(argc != 3)
    {
        printf("Zle parametry wywolania. Sprobuj ./rm [nazwa_dysku] [nazwa_pliku]\n");
        return 1;
    }
    rm(argv[1],argv[2]);
    return 0;
}
