#ifndef __WEBHDFS_INCLUDE__
#define __WEBHDFS_INCLUDE__

#include <ghttp.h>

#define IP_LEN 16
#define MAX_ADDRESS_NUM 2
#define MAX_USER_NAME_LEN 32

typedef struct s_webhdfs webhdfs_t;

typedef int (*hdfs_op)(webhdfs_t* hdfs, char* );
typedef int (*hdfs_read)(webhdfs_t* hdfs, char* filename, char* buf, long long offset, int len);
typedef int (*hdfs_write)(webhdfs_t* hdfs, char* filename, char* buf, int len);
typedef int (*hdfs_status)(webhdfs_t* hdfs);

typedef struct s_hdfs_address
{
    char    ip[IP_LEN];
    int     port;
} hdfs_address_t;

struct s_webhdfs
{
    hdfs_address_t  address_[MAX_ADDRESS_NUM];
    ghttp_request*  handle_;
    int             index_;
    char            user_[MAX_USER_NAME_LEN];

    hdfs_op         create_file;
    hdfs_op         delete_file;
    hdfs_read       read;
    hdfs_write      write;
    hdfs_status     status;
    hdfs_op         create_dir;
    hdfs_op         delete_dir;
};

webhdfs_t*  webhdfs_t_create(hdfs_address_t* addr, int num, const char* user);
int         webhdfs_t_destroy(webhdfs_t* hdfs);

int webhdfs_create_file(webhdfs_t* hdfs, char* filename);
int webhdfs_delete_file(webhdfs_t* hdfs, char* filename);
int webhdfs_read(webhdfs_t* hdfs, char* filename, char* buf, long long offset, int len);
int webhdfs_write(webhdfs_t* hdfs, char* filename, char* buf, int len);
int webhdfs_status(webhdfs_t* hdfs);
int webhdfs_create_dir(webhdfs_t* hdfs, char* dir);
int webhdfs_delete_dir(webhdfs_t* hdfs, char* dir);

#endif