/* sbrk and brk example */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    void *curr_brk = NULL;

    printf("Welcome to sbrk example:%d\n", getpid());

    /* sbrk(0) gives current program break location */
    curr_brk = sbrk(0);
    printf("Program Break Location1:%p\n", curr_brk);
    getchar();

    char* p = new char[4096];

    for (int i = 0; i < 4096; ++i)
    {
        p[i] = 'A';
    }

    curr_brk = sbrk(0);
    printf("Program break Location2:%p\n", curr_brk);
    getchar();

    delete []p;

    curr_brk = sbrk(0);
    printf("Program Break Location3:%p\n", curr_brk);
    getchar();

    return 0;
}