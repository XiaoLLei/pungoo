#include "disk.h"
#include "def.h"

#define LEN (64 * 1024 * 1024)

int main()
{
    LOG("hello, world!");

    char filename[256] = {0};

    char* buff = (char*)malloc(LEN + 1);

    int time = 0;
    for (; time < 10000000; ++time)
    {
        memset(buff, 0, LEN + 1);

        snprintf(filename, sizeof(filename), "./cache/data%05d", time);

        disk_t* disk = disk_t_create(filename, FILE_OPEN_MOD_READ);
        if (!disk)
        {
            break;
        }

        if (0 != disk->open(disk))
        {
            disk_t_destroy(disk);
            break;
        }

        if (LEN != disk->read(disk, buff, LEN))
        {
            disk_t_destroy(disk);
            break;
        }

        disk_t_destroy(disk);

        usleep(100);
    }

    free(buff);

    return 0;
}
