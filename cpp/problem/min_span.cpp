// 题目：有一串首尾相连的珠子，共有N个，每一个珠子有一种颜色，求连续的珠子的颜色总数为M时，长度最小的区间。

#include <iostream>
#include <map>
using namespace std;

void printmap(map<int, int>& data)
{
    for (map<int, int>::iterator it = data.begin(); it != data.end(); ++it)
    {
        cout << it->first << "(" << it->second << ")  ";
    }
    cout << endl;
}

class Solution
{
public:
    int span_counter(int* arr, int n, int m, int& first)
    {
        int bestlen = n + 1;
        int bestidx = -1;

        map<int, int> mapColor;

        do
        {
            if (n < m)
                break;

            int lastidx = 0;

            for (int i = 0; i < 2 * n; ++i)
            {
                int valid_idx = i % n;
                ++mapColor[arr[valid_idx]];

                if (m == mapColor.size())
                {
                    int curlen1 = (i - lastidx + 1) % n;

                    bool flag = false;

                    for (int j = 0; j < curlen1 - m + 1; ++j)
                    {
                        if (lastidx + 1 >= n)
                        {
                            flag = true;
                            break;
                        }

                        int color = arr[lastidx];
                        lastidx = (lastidx + 1) % n;

                        if (1 < mapColor[color])
                        {
                            --mapColor[color];
                            int curlen2 = (i - lastidx + 1) % n;
                            if (bestlen > curlen2)
                            {
                                bestlen = curlen2;
                                bestidx = lastidx;
                            }
                        }
                        else
                        {
                            mapColor.erase(color);
                            break;
                        }
                    }

                    if (flag)
                    {
                        cout << "flag is effective. i = " << i << endl;
                        break;
                    }
                }

            }
        }
        while (false);

        first = bestidx;

        return bestlen;
    }
};

int main()
{
    int arr[] = { 1, 3, 3, 3, 4, 4, 3, 3, 4, 3, 4, 3, 3, 1, 2, 3, 4};
    Solution s;
    int first = -1;
    cout << "LEN : " << s.span_counter(arr, sizeof(arr), 4, first) << endl;
    cout << "IDX : " << first << endl;

    for (int i = 0; i < sizeof(arr); ++i)
    {
    }

    return 0;
}