#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#include <inttypes.h>
#include <iostream>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <stdint.h>
#include <amqp.h>
#include <amqp_framing.h>

#include "utils.h"

void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(1);
}

void die_on_error(int x, char const *context)
{
    if (x < 0)
    {
        fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x));
        exit(1);
    }
}

void die_on_amqp_error(amqp_rpc_reply_t x, char const *context)
{
    switch (x.reply_type)
    {
    case AMQP_RESPONSE_NORMAL:
        return;

    case AMQP_RESPONSE_NONE:
        fprintf(stderr, "%s: missing RPC reply type!\n", context);
        break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
        fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x.library_error));
        break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
        switch (x.reply.id)
        {
        case AMQP_CONNECTION_CLOSE_METHOD:
        {
            amqp_connection_close_t *m = (amqp_connection_close_t *) x.reply.decoded;
            fprintf(stderr, "%s: server connection error %uh, message: %.*s\n",
                    context,
                    m->reply_code,
                    (int) m->reply_text.len, (char *) m->reply_text.bytes);
            break;
        }
        case AMQP_CHANNEL_CLOSE_METHOD:
        {
            amqp_channel_close_t *m = (amqp_channel_close_t *) x.reply.decoded;
            fprintf(stderr, "%s: server channel error %uh, message: %.*s\n",
                    context,
                    m->reply_code,
                    (int) m->reply_text.len, (char *) m->reply_text.bytes);
            break;
        }
        default:
            fprintf(stderr, "%s: unknown server error, method id 0x%08X\n", context, x.reply.id);
            break;
        }
        break;
    }

    exit(1);
}

static void dump_row(long count, int numinrow, int *chs)
{
    int i;

    printf("%08lX:", count - numinrow);

    if (numinrow > 0)
    {
        for (i = 0; i < numinrow; i++)
        {
            if (i == 8)
            {
                printf(" :");
            }
            printf(" %02X", chs[i]);
        }
        for (i = numinrow; i < 16; i++)
        {
            if (i == 8)
            {
                printf(" :");
            }
            printf("   ");
        }
        printf("  ");
        for (i = 0; i < numinrow; i++)
        {
            if (isprint(chs[i]))
            {
                printf("%c", chs[i]);
            }
            else
            {
                printf(".");
            }
        }
    }
    printf("\n");
}

static int rows_eq(int *a, int *b)
{
    int i;

    for (i = 0; i < 16; i++)
        if (a[i] != b[i])
        {
            return 0;
        }

    return 1;
}

void amqp_dump(void const *buffer, size_t len)
{
    unsigned char *buf = (unsigned char *) buffer;
    long count = 0;
    int numinrow = 0;
    int chs[16];
    int oldchs[16] = {0};
    int showed_dots = 0;
    size_t i;

    for (i = 0; i < len; i++)
    {
        int ch = buf[i];

        if (numinrow == 16)
        {
            int j;

            if (rows_eq(oldchs, chs))
            {
                if (!showed_dots)
                {
                    showed_dots = 1;
                    printf("          .. .. .. .. .. .. .. .. : .. .. .. .. .. .. .. ..\n");
                }
            }
            else
            {
                showed_dots = 0;
                dump_row(count, numinrow, chs);
            }

            for (j = 0; j < 16; j++)
            {
                oldchs[j] = chs[j];
            }

            numinrow = 0;
        }

        count++;
        chs[numinrow++] = ch;
    }

    dump_row(count, numinrow, chs);

    if (numinrow != 0)
    {
        printf("%08lX:\n", count);
    }
}


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

const char*   g_szHostname = "172.18.32.55";
int     g_nPort = 5672;
const char*   g_szVirtualhost = "/";
const char*   g_szExchange = "amq.direct";
const char*   g_szRoutingkey = "test";
const char*   g_szUser = "guest";
const char*   g_szPassword = "guest";

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

    if (argc > 1)
        g_szHostname = argv[2];

    if (argc > 2)
        g_nPort = atoi(argv[3]);

    if (argc > 3)
        g_szExchange = argv[4];

    if (argc > 4)
        g_szRoutingkey = argv[5];

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
            printf("%10ld TIMES %5ld TICK %10.2lf QPS\n", info.count, tick, (double)info.count / tick);
        }
    }

    pthread_join(tid, NULL);

    return 0;
}

void* thread_loop(void* arg)
{
    ThreadInfo& info = *(ThreadInfo*)arg;

    // pid_t thread_id = getThreadId();

    amqp_connection_state_t conn = amqp_new_connection();

    amqp_socket_t *socket = amqp_tcp_socket_new(conn);

    if (!socket)
    {
        die("creating TCP socket");
    }

    int status = amqp_socket_open(socket, g_szHostname, g_nPort);
    if (status)
    {
        die("opening TCP socket");
    }

    die_on_amqp_error(amqp_login(conn, g_szVirtualhost, 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, g_szUser, g_szPassword),
                      "Logging in");
    amqp_channel_open(conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");


    const int kLen = 1024 * 10;

    std::string strValue;
    for (int i = 0; i < kLen; ++i)
    {
        strValue.push_back('A');
    }

    int idx = 0;

    while (true)
    {
        char queue_name[32] = {0};
        snprintf(queue_name, 32, "%s_%ld", g_szRoutingkey, ++idx % 10000);

        amqp_basic_properties_t props;
        props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_EXPIRATION_FLAG;
        props.content_type = amqp_cstring_bytes("text/plain");
        props.delivery_mode = 2; /* persistent delivery mode */
        props.expiration = amqp_cstring_bytes("1");
        die_on_error(
            amqp_basic_publish(
                conn,
                1,
                amqp_cstring_bytes(g_szExchange),
                amqp_cstring_bytes(queue_name),
                0,
                0,
                &props,
                amqp_cstring_bytes(strValue.c_str())),
            "Publishing");

        sleep(1);

        InterlockedIncrement64(&info.count);
    }

    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS), "Closing connection");
    die_on_error(amqp_destroy_connection(conn), "Ending connection");

    return NULL;
}
