#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;


int main()
{
    char str[] = "Hello, world!";

    pid_t pid = fork();
    switch(pid)
    {
    case 0:
        {
            cout << "parent:" << (size_t)&(str[0]) << endl;
            sleep(5);
            cout << "parent:" << (size_t)&(str[0]) << endl;
        }
        break;
    case -1:
        break;
    default:
        {
            sleep(1);
            cout << "child:" << (size_t)&(str[0]) << endl;
            str[0] = 'D';
            cout << "child:" << (size_t)&(str[0]) << endl;
            sleep(5);
        }
        break;
    }
    return 0;
}