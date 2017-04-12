/*************************************************************************
    > File Name: ipv4.cpp
    > Author: XiaoLLei
    > Mail: Errant.Xia@gmail.com
    > Created Time: Wed 12 Apr 2017 01:31:22 PM CST
 ************************************************************************/

#include <iostream>
#include <stdio.h>
using namespace std;


bool IsValidIpv4(const char* ip)
{
    int num[4] = { -1, -1, -1, -1};
    int idx = 0;
    const char* p = ip;
    while ('\0' != *p)
    {
        int x = -1;
        const char* q = p;
        for (int i = 0; i < 4; ++i)
        {
            if ('.' == *q)
            {
                ++q;
                break;
            }
            else if ('\0' == *q)
            {
                break;
            }
            else
            {
                int tmp = *q - '0';
                if (0 == tmp && 0 == x)
                {
                    x = -1;
                    break;
                }
                else if (tmp > 9 || tmp < 0)
                {
                    x = -1;
                    break;
                }
                else
                {
                    if (-1 == x)
                    {
                        x = tmp;
                    }
                    else if (0 == x)
                    {
                        x = -1;
                        break;
                    }
                    else
                    {
                        x = x * 10 + tmp;
                    }
                    if (x > 255)
                    {
                        x = -1;
                        break;
                    }
                }
            }

            ++q;
        }

        if (-1 != x)
        {
            if (0 == idx && 0 == x)
            {
                return false;
            }

            num[idx] = x;
            ++idx;
            p = q;

            if (4 == idx && '\0' != *p)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "params is not correct" << endl;
        return -1;
    }

    const char* pszIpv4 = argv[1];

    if (IsValidIpv4(pszIpv4))
    {
        printf("[success] [%s] is a valid Ipv4 address.\n", pszIpv4);
        return 0;
    }
    else
    {
        printf("[error] [%s] is not a valid Ipv4 address.\n", pszIpv4);
        return -1;
    }
}

