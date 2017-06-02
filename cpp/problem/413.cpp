#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int numberOfArithmeticSlices(vector<int>& A) {
        if (A.size() < 3)
            return 0;
        
        int ret = 0;
        bool flag = false;
        int gap = -1;
        int first = 0;
        for (int i = 1; i < A.size(); )
        {
            if (!flag)
            {
                flag = true;
                gap = A[i] - A[i-1];
                first = i - 1;
                cout << "first : " << first << endl;
                cout << "gap : " << gap << endl;
                ++i;
            }
            else if (gap != A[i] - A[i-1])
            {
                if (i - first >= 3)
                {
                    int num = i - first;
                    cout << "num = " << num << " first: " << first << " i: " << i << endl;
                    ret += (num - 2) * (num - 1) / 2;
                    cout << "1:  " << ret << endl;
                }
                flag = false;
            }
            else
            {
                ++i;
            }
        }
        
        if (flag && first + 3 <= A.size())
        {
            int num = A.size() - first;
            ret += (num - 2) * (num - 1) / 2;
            cout << "2:  " << ret << endl;
        }
        return ret;
    }
};

int main()
{
    int a[] = {2,1,3,4,2,3};
    // int a[] = {1,2,3,4};
    std::vector<int> v;
    for (int i = 0; i < sizeof(a) / sizeof(int); ++i)
    {
        v.push_back(a[i]);
    }
    Solution s;
    cout << s.numberOfArithmeticSlices(v) << endl;
    return 0;
}