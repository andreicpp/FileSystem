#include "op.h"

void diag(char * vdisk)
{
    FILE* disk = fopen(vdisk,"rb");
    int inodeInBlock;
    char stan;
    int inodesInUse;
    int i,j;
    superblock sb;
    char * dataBitmap;
    char * inodeBitmap;
    int *dataBlocksSize;
    short *dataPointers;
    inode node;
    int dataSize;
    disk_read(disk,sizeof(superblock),&sb);
    
    dataBitmap = (char *)malloc(sb.number_of_blocks);
    inodeBitmap = (char *)malloc(sb.number_of_blocks);
    disk_seek(disk,sb.inodeBitmapAddr);
    disk_read(disk,sb.number_of_blocks,inodeBitmap);
    disk_seek(disk,sb.dataBitmapAddr);
    disk_read(disk,sb.number_of_blocks,dataBitmap);
    inodeInBlock = BLOCK_SIZE/sb.inodeSize;
    
    printf("0\t\tS\t\t%d/%dkB\n",sizeof(superblock),BLOCK_SIZE);
    printf("1\t\ti\t\t%d/%dkB\n",sizeof(short)*sb.number_of_blocks,BLOCK_SIZE);
    printf("2\t\td\t\t%d/%dkB\n",sizeof(short)*sb.number_of_blocks,BLOCK_SIZE);
    for(i = 0; i < sb.inodeSectorSize/BLOCK_SIZE-1; i++)
    {
        inodesInUse = 0;
        for(j = 0;j < inodeInBlock; j++)
            if(inodeBitmap[i*inodeInBlock+j] == '1')
                inodesInUse++;
        printf("%d\t\tI\t\t%d/%dkB\t\t\t%d/%d\n",2+i,inodesInUse*sb.inodeSize,BLOCK_SIZE,inodesInUse,inodeInBlock+2);
    }
    
//    inodesInUse = 0;
//    inodeInBlock = sb.number_of_inodes-i*inodeInBlock;
//    for(j = 0;j < inodeInBlock; j++)
//        if(inodeBitmap[i*BLOCK_SIZE/sb.inodeSize+j] == '1')
//                inodesInUse++;
//    printf("%d\t\tI\t\t%d/%dkB\t\t\t%d/%d\n",2+i,inodesInUse*sb.inodeSize,BLOCK_SIZE,inodesInUse,inodeInBlock);
    
    dataBlocksSize = (int *)malloc(sizeof(int)*sb.number_of_blocks);
    dataPointers = (short *)malloc(sizeof(short)*sb.number_of_blocks);
    for(i = 0;i < sb.number_of_inodes; i++)
    {
        if(inodeBitmap[i] == '1')
        {
            disk_seek(disk,sb.inodeTableAddr+i*sb.inodeSize);
            disk_read(disk,sizeof(inode),&node);
            disk_read(disk,sb.number_of_blocks*sizeof(short),dataPointers);
            dataSize = node.dataSize;
            for(j = 0;dataSize > BLOCK_SIZE;j++)
            {
                dataBlocksSize[dataPointers[j]] = BLOCK_SIZE;
                dataSize -= BLOCK_SIZE;
            }
            dataBlocksSize[dataPointers[j]] = dataSize;
        }
    }
    for(i = 0; i < sb.number_of_blocks; i++)
    {
        stan = (dataBitmap[i] == '1')?'Z':'W';
        if(stan == 'Z')
            printf("%d\t\tD\t\t%d/%dkB\t\t\t%c\n",i-2+sb.dataAddr/BLOCK_SIZE,dataBlocksSize[i],BLOCK_SIZE,stan);
        else
            printf("%d\t\tD\t\t0/%dkB\t\t\t%c\n",i-2+sb.dataAddr/BLOCK_SIZE,BLOCK_SIZE,stan);
    }
    
    fclose(disk);
}

int main(int argc,char** argv)
{
    
    if(argc == 2 && !strcmp(argv[1],"--help"))
        printf("Diagnostyka dysku. \nTypy: \nS - superblock, \ni - bitmapa Inode, \nd - bitmapa danych, \nI - blok inode'ow, \nD - blok danych\n\nStany: x/y - ilosc zajetych inode'ow na ilosc ich w bloku(dla blokow inode), \nZ - zajety, \nW - wolny (oba dla blokow danych)\n");
    else if(argc != 2)
    {
        printf("Zle parametry wywolania. Sprobuj ./diag [nazwa_dysku]\n");
        return 1;
    }
    else
    {
        printf("adres\t\ttyp\t\trozmiar\t\t\tstan\n");
        diag(argv[1]);
    }
    return 0;
}
