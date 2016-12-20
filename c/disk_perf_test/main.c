#include "disk.h"
#include "def.h"

int main()
{
    LOG("hello, world!");

    disk_t* disk = disk_t_create("./disk.c", FILE_OPEN_MOD_WRITE);
    if (!disk)
    {
        return -1;
    }

    if (0 != disk->open(disk))
    {
        disk_t_destroy(disk);
        return -1;
    }

    char buff[256] = {0};
    if (0 != disk->read(disk, buff, 256))
    {
        disk_t_destroy(disk);
        return -1;
    }

    printf("%s", buff);

    disk_t_destroy(disk);

    return 0;
}