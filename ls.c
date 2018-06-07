#include "op.h"

void list_files(char * vdisk)
{
    FILE* disk = fopen(vdisk,"rb");
    inode node;
    char byte;
    int i;
    char* inodeBitmap;
    int* inodes;
    superblock sb;
    int inodes_index;
    disk_read(disk,sizeof(superblock),&sb);
    inodes = (int *)malloc((sb.number_of_inodes-sb.freeInodes)*sizeof(int));
    inodes_index = 0;
    disk_seek(disk,sb.inodeBitmapAddr);
    inodeBitmap = (char *)malloc(sb.number_of_blocks);
    disk_seek(disk,sb.inodeBitmapAddr);
    disk_read(disk,sizeof(char)*sb.number_of_blocks,inodeBitmap);
    for(i = 0; i < sb.number_of_inodes; i++)
    {
        byte = inodeBitmap[i];
        if(byte == '1')
            inodes[inodes_index++] = i;
    }
    printf("nazwa\t\trozmiar\n");
    for(i = 0; i < inodes_index; i++)
    {
        disk_seek(disk,sb.inodeTableAddr+inodes[i]*sb.inodeSize);
        disk_read(disk,sizeof(inode),&node);
        printf("%s\t\t%d\n",node.filename,node.dataSize);
    }
    fclose(disk);
}

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        printf("Zle parametry wywolania. Sprobuj ./ls [nazwa_dysku]\n");
        return 1;
    }
    list_files(argv[1]);
    return 0;
}
