#include "disk.h"
#include "def.h"

#define LEN (1 * 1024 * 1024)

int main()
{
    LOG("hello, world!");

    char filename[256] = {0};

    char* buff = (char*)malloc(LEN);

    char* p = buff;
    int i = 0;
    for (i = 0; i < LEN; ++i)
    {
        *p = i & 0x7F;
        ++p;
    }

    int time = 0;
    for (; time < 10000000; ++time)
    {
        snprintf(filename, sizeof(filename), "./cache/data%05d", time);

        disk_t* disk = disk_t_create(filename, FILE_OPEN_MOD_WRITE);
        if (!disk)
        {
            return -1;
        }

        if (0 != disk->open(disk))
        {
            disk_t_destroy(disk);
            return -1;
        }

        if (LEN != disk->write(disk, buff, LEN))
        {
            disk_t_destroy(disk);
            return -1;
        }

        disk_t_destroy(disk);
    }

    return 0;
}
