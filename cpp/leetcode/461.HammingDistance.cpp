/*************************************************************************
	> File Name: 461.HammingDistance.cpp
	> Author: XiaoLLei
	> Mail: Errant.Xia@gmail.com
	> Created Time: Tue 24 Jan 2017 11:24:11 AM CST
 ************************************************************************/

#include <iostream>
#include <stdlib.h>
using namespace std;

class Solution {
public:
    int hammingDistance(int x, int y) {
        if (x == y) {
            return 0;
        } else {
            int tmp = (x ^ y);
            cout << tmp << endl;
            int count = 0;
            for (int i = 0; i < 31; ++i) {
                if (tmp & 0x1) {
                    ++count;
                }
                tmp = (tmp >> 1);
            }
            return count;
        }
    }
};

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "wrong params" << endl;
        return 1;
    }
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);
    Solution s;
    cout << s.hammingDistance(x,y) << endl;;
    return 0;
}
