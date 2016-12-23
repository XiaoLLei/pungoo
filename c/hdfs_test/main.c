#include "def.h"
#include "webhdfs.h"

int main()
{
    hdfs_address_t addr[2];

    strncpy(addr[0].ip, "10.80.5.21", IP_LEN - 1);
    addr[0].port = 50070;

    strncpy(addr[1].ip, "10.80.5.22", IP_LEN - 1);
    addr[1].port = 50070;

    webhdfs_t* hdfs = webhdfs_t_create(addr, 2, "hdfs");

    if (!hdfs)
    {
        return -1;
    }

    if (0 != hdfs->create_file(hdfs, "/hello_world"))
    {
        LOG("create hello_world failure!");
    }
    else
    {
        LOG("create hello_world successfully!");
    }

    webhdfs_t_destroy(hdfs);

    return 0;
}