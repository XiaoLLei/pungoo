#include "r3c.h"
#include <inttypes.h>
#include <iostream>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define nouse(K) {if(K){}};

int64_t InterlockedAdd64(int64_t i, int64_t *v)
{
#define LOCK_PREFIX "lock ; "

    int64_t __i = i;
    __asm__ __volatile__(
        LOCK_PREFIX "xaddq %0, %1;"
        :"=r"(i)
        :"m"(*v), "0"(i));
    return i + __i;
}

#define InterlockedIncrement64(v)  (InterlockedAdd64(1ll,v))

pid_t getThreadId()
{
    return syscall(SYS_gettid);
}

static void my_log_write(const char* format, ...)
{
    return;
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
}

void* thread_loop(void* arg);

struct ThreadInfo
{
    int64_t count;
};

int main(int argc, char* argv[])
{
    // nouse(argc);
    // nouse(argv);

    int thread_num = 1;
    if (argc > 1)
    {
        thread_num = atoi(argv[1]);
    }

    r3c::set_info_log_write(my_log_write);
    r3c::set_debug_log_write(my_log_write);

    ThreadInfo info;

    pthread_t tid = 0;
    for (int i = 0; i < thread_num; ++i)
    {
        pthread_create(&tid, NULL, thread_loop, &info);
    }

    long start_time = time(0);
    long last_time = start_time;
    while (true)
    {
        if (time(0) - last_time >= 1)
        {
            last_time = time(0);
            long tick = last_time - start_time;
            printf("%10ld TIMES %5ld TICK %10.2lf QPS\n", info.count, tick, (double)info.count/tick);
        }
    }

    pthread_join(tid, NULL);

    return 0;
}

void* thread_loop(void* arg)
{
    ThreadInfo& info = *(ThreadInfo*)arg;

    pid_t thread_id = getThreadId();

    try
    {
        // const char* cmd = argv[1];
        // const char* key = argv[2];
        // const char* nodes = getenv("HOSTS");
        //const char* nodes = "172.18.32.124:7000,172.18.32.124:7001,172.18.32.124:7002,172.18.32.124:7003,172.18.32.124:7004,172.18.32.124:7005";
        // const char* nodes = "172.18.32.55:7001,172.18.32.55:7002,172.18.32.55:7003,172.18.32.55:7004,172.18.32.55:7005,172.18.32.55:7006";
        const char* nodes = "172.18.32.55:6380";
        if (NULL == nodes)
        {
            fprintf(stderr, "Environment[HOSTS] not set, example: export HOSTS=127.0.0.1:6379,127.0.0.1:6380,127.0.0.1:6381\n");
            exit(1);
        }
        if ('\0' == *nodes)
        {
            fprintf(stderr, "Environment[HOSTS] without value, example: export HOSTS=127.0.0.1:6379,127.0.0.1:6380,127.0.0.1:6381\n");
            exit(1);
        }

        r3c::CRedisClient redis_client(nodes);

        const int kLen = 1024 * 50;

        std::string strValue;
        for (int i = 0; i < kLen; ++i)
        {
            strValue.push_back('A');
        }

        long count = 0;

        while (true)
        {
            std::pair<std::string, uint16_t> which_node;
            // int count = redis_client.rpush("mylist", argv[0], &which_node);
            // printf("mylist has %d items. %d--%s\n", count, which_node.second, which_node.first.c_str());
            char key[64] = {0};
            snprintf(key, 64, "%d-%ld", thread_id, ++count);
            redis_client.setex(key, strValue, 1, &which_node);
            //printf("%s. %d--%s\n", key, which_node.second, which_node.first.c_str());
            //dd//sleep(1);

            InterlockedIncrement64(&info.count);
        }

    }
    catch (r3c::CRedisException& ex)
    {
        fprintf(stderr, PRINT_COLOR_RED"%s"PRINT_COLOR_NONE"\n", ex.str().c_str());
        exit(1);
    }


    return NULL;
}