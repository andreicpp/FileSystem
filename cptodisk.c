#include "op.h"

void copy_to_disk(char * vdisk, char * filename, char * newname)
{
    FILE* disk = fopen(vdisk,"r+b");
    FILE* cpfile = fopen(filename,"rb");
    char buffer[BLOCK_SIZE];
    char byte;
    inode node;
    int i;
    int inodePointer;
    int dataBlockIndex;
    char *inodeBitmap;
    char *dataBitmap;
    int filesize;
    int blocks_needed;
    struct stat st;
    superblock sb;
    short int * blocks;
    stat(filename,&st); // for file size
    filesize = st.st_size;
    blocks_needed = ceil(1.0*filesize/BLOCK_SIZE); //how much blok needed for file
    disk_read(disk,sizeof(superblock),&sb);
    inodeBitmap = (char *)malloc(sb.number_of_blocks);
    dataBitmap = (char *)malloc(sb.number_of_blocks);
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
            if(strcmp(node.filename,newname) == 0)
            {
                printf("Taki plik juz istnieje!\n");
                fclose(disk);
                fclose(cpfile);
                return;
            }
        }

    }
    if(blocks_needed <= sb.freeDataBlocks && sb.freeInodes > 0)
    {
        blocks = (short int *)malloc(sb.number_of_blocks*sizeof(short int)); // !!!!!!
        dataBlockIndex = 0;
        for(i = 0; i < sb.number_of_blocks && blocks_needed > 0; i++)
        {
            byte = dataBitmap[i];
            if(byte == '0')
            {
                dataBitmap[i] = '1';
                blocks[dataBlockIndex++] = i;
                blocks_needed--;
            }
        }
        for(i = 0; i < sb.number_of_inodes; i++)
        {
            byte = inodeBitmap[i];
            if(byte == '0')
            {
                inodeBitmap[i] = '1';
                inodePointer = i;
                break;
            }
        }
        disk_seek(disk,sb.inodeTableAddr+inodePointer*sb.inodeSize);
        strcpy(node.filename,newname);
        node.dataSize = filesize;
        disk_write(disk,sizeof(inode),&node);
        disk_write(disk,sb.number_of_blocks*sizeof(short),blocks);  //
        printf("%s\t\t%d\n",node.filename,node.dataSize);           // file name + bytes
        for(i = 0; i < dataBlockIndex; i++)
        {
            disk_seek(disk,sb.dataAddr+blocks[i]*BLOCK_SIZE);       //
            disk_read(cpfile,BLOCK_SIZE,buffer);                    //
            disk_write(disk,BLOCK_SIZE,buffer);                     //
        }
        fill_buff(BLOCK_SIZE,buffer,'0');                           // fill buff with 0
        disk_write(disk,BLOCK_SIZE-filesize%BLOCK_SIZE,buffer);
        sb.freeDataBlocks -= ceil(1.0*st.st_size/BLOCK_SIZE);
        sb.freeInodes--;
        disk_seek(disk,0);
        disk_write(disk,sizeof(superblock),&sb);
        disk_seek(disk,sb.inodeBitmapAddr);
        disk_write(disk,sb.number_of_blocks,inodeBitmap);
        disk_seek(disk,sb.dataBitmapAddr);
        disk_write(disk,sb.number_of_blocks,dataBitmap);
    }
    else
        printf("Za duzo plikow na dysku, lub plik jest za duzy!\n");
    fclose(disk);
    fclose(cpfile);
    free(blocks);
}

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        printf("Zle parametry wywolania. Sprobuj ./copytodisk [nazwa_dysku] [nazwa_pliku] [nowa_nazwa_pliku]\n");
        return 1;
    }
    copy_to_disk(argv[1],argv[2],argv[3]);
    return 0;
}
