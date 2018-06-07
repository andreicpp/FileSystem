#include "op.h"

void disk_seek(FILE* disk, int offset)
{
    fseek(disk,offset,0);
}

void disk_read(FILE* disk, int size, void* buff)
{
    fread(buff,size,1,disk);
}

void disk_write(FILE* disk, int size, void *buff)
{
    fwrite(buff,size,1,disk);
}

void fill_buff(int size, char *buff, char character)
{
    int i;
    for (i = 0; i < size; i++)
        buff[i] = character;
}
