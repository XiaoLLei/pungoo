#ifndef __DEF_INCLUDE__
#define __DEF_INCLUDE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define LOG(fmt, ...) \
    {   \
        printf("[%s:%d] [%s] " fmt "\n", \
            __FILE__, \
            __LINE__, \
            __PRETTY_FUNCTION__, \
            ##__VA_ARGS__); \
    }


#endif