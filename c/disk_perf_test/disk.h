#ifndef __DISK_INCLUDE__
#define __DISK_INCLUDE__

#include <stdio.h>


typedef FILE * file_handle_t;

#define MAX_PATH_LEN 256

typedef struct s_disk disk_t;

typedef int (*file_oc)(disk_t* disk);
typedef int (*file_rw)(disk_t* disk, char* buf, int len);
typedef int (*file_seek)(disk_t* disk, long long offset);

enum
{
    FILE_OPEN_MOD_WRITE         = 0,
    FILE_OPEN_MOD_WRITE_APEND   = 1,
    FILE_OPEN_MOD_READ          = 2,
};

struct s_disk
{
    char            filename_[MAX_PATH_LEN];
    file_handle_t   handle_;
    int             flag_;

    file_oc         open;
    file_oc         close;
    file_rw         read;
    file_rw         write;
    file_seek       seek;
};

disk_t* disk_t_create(char* filename, int flag);
int     disk_t_destroy(disk_t* disk);

int     disk_t_file_open(disk_t* disk);
int     disk_t_file_close(disk_t* disk);
int     disk_t_file_read(disk_t* disk, char* buf, int len);
int     disk_t_file_write(disk_t* disk, char* buf, int len);
int     disk_t_file_seek(disk_t* disk, long long offset);

#endif