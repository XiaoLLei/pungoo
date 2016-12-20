#include "def.h"
#include "disk.h"

disk_t* disk_t_create(char* filename, int flag)
{
    if (NULL == filename || strlen(filename) >= MAX_PATH_LEN)
    {
        LOG("wrong file name. filename: %s", filename);
        return NULL;
    }
    else if (strlen(filename) >= MAX_PATH_LEN)
    {
        LOG("too long file path. filename: %s", filename);
        return NULL;
    }

    disk_t* disk = (disk_t*)malloc(sizeof(disk_t));
    if (NULL == disk)
    {
        LOG("malloc mem for disk_t failure. filename: %s", filename);
    }

    memset(disk, 0, sizeof(disk_t));
    strncpy(disk->filename_, filename, MAX_PATH_LEN - 1);
    disk->flag_ = flag;

    disk->open = disk_t_file_open;
    disk->close = disk_t_file_close;
    disk->read = disk_t_file_read;
    disk->write = disk_t_file_write;
    disk->seek = disk_t_file_seek;

    return disk;
}

int disk_t_destroy(disk_t* disk)
{
    if (disk)
    {
        disk->close(disk);
        free(disk);
    }
    return 0;
}

int disk_t_file_open(disk_t* disk)
{
    if (!disk)
    {
        LOG("empty disk object.");
        return -1;
    }

    if (FILE_OPEN_MOD_WRITE == disk->flag_)
    {
        disk->handle_ = fopen(disk->filename_, "wb");
    }
    else if (FILE_OPEN_MOD_WRITE_APEND == disk->flag_)
    {
        disk->handle_ = fopen(disk->filename_, "ab");
    }
    else
    {
        disk->handle_ = fopen(disk->filename_, "rb");
    }

    if (!disk->handle_) {
        LOG("open file failure. errno: %d, filename: %s", errno, disk->filename_);
        return -1;
    }

    return 0;
}

int disk_t_file_close(disk_t* disk)
{
    if (!disk)
    {
        LOG("empty disk object.");
        return -1;
    }

    if (disk->handle_)
    {
        if (fclose(disk->handle_) != 0)
        {
            LOG("close file failure. errno: %d, filename: %s", errno, disk->filename_);
            return -1;
        }
        disk->handle_ = NULL;
        return 0;
    }

    LOG("file is not opened. filename: %s", disk->filename_);
    return -1;
}

int disk_t_file_read(disk_t* disk, char* buf, int len)
{
    int read_len = 0, n = 0;
    int retry = 0;
    int err = 0;

    if (!disk)
    {
        LOG("empty disk object.");
        return -1;
    }

    while (read_len < len)
    {
        n = fread(buf + read_len, 1, len - read_len, disk->handle_);
        err = ferror(disk->handle_);
        if (0 == n && err != EAGAIN && err != EINTR && !feof(disk->handle_))
        {
            LOG("file read failed, read_len: %d != len: %d , errno: %d, filename: %s",
                read_len, len, err, disk->filename_);
            return -1;
        }
        else if (0 == n)
        {
            if (feof(disk->handle_))
            {
                LOG("file read end , errno: %d, filename: %s", err, disk->filename_);
                return read_len;
            }
            //尝试一定次数
            retry++;
            if (retry < 6000)
            {
                usleep(10000);
            }
            else
            {
                return -1;
            }
            LOG("file read failed, retry: %d, filename: %s", retry, disk->filename_);
        }
        else
        {
            read_len += n;
        }
    }

    return read_len;
}

int disk_t_file_write(disk_t* disk, char* buf, int len)
{
    int write_len = 0, n = 0;
    int retry = 0;

    if (!disk)
    {
        LOG("empty disk object.");
        return -1;
    }

    while (write_len < len)
    {
        n = fwrite(buf + write_len, 1, len - write_len, disk->handle_);
        if (0 == n && ferror(disk->handle_) != EAGAIN && ferror(disk->handle_) != EINTR)
        {
            LOG("file write failed, write_len: %d != len: %d failed, errno: %d", write_len, len, ferror(disk->handle_));
            return 0;
        }
        else if (0 == n)
        {
            //尝试一定次数
            ++retry;
            if (retry < 6000)
            {
                usleep(10000);
            }
            else
            {
                return 0;
            }
            LOG("file write failed, retry: %d, file: %s", retry, disk->filename_);
        }
        else
        {
            write_len += n;
        }
    }

    return write_len;
}

int disk_t_file_seek(disk_t* disk, long long offset)
{
    if (!disk)
    {
        LOG("empty disk object.");
        return -1;
    }

    if (-1 == fseek(disk->handle_, offset, SEEK_SET))
    {
        LOG("file seek failed, offset: %lld , errno: %d, filename: %s", offset, errno, disk->filename_);
        return -1;
    }

    return 0;
}
