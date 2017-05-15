#include <iostream>
#include <stdio.h>
using namespace std;

class CHeapSort
{
public:
    void sort(int* arr, int n)
    {
        for (int i = n / 2; i >= 0; --i)
        {
            heapAjust(arr, n, i);
        }

        for (int i = n - 1; i > 0; --i)
        {
            int temp = arr[0];
            arr[0] = arr[i];
            arr[i] = temp;

            heapAjust(arr, i, 0);
        }
    }

private:
    void heapAjust(int* arr, int n, int pos)
    {
        if (pos >= n)
        {
            return;
        }

        for (int i = pos * 2; i < n; i = pos * 2)
        {
            int temp = arr[pos];
            if (i + 1 < n)
            {
                if (temp < arr[i] || temp < arr[i + 1])
                {
                    if (arr[i] < arr[i + 1])
                    {
                        arr[pos] = arr[i + 1];
                        arr[i + 1] = temp;
                        pos = i + 1;
                    }
                    else
                    {
                        arr[pos] = arr[i];
                        arr[i] = temp;
                        pos = i;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (temp < arr[i])
                {
                    arr[pos] = arr[i];
                    arr[i] = temp;
                    pos = i;
                }
                else
                {
                    break;
                }
            }
        }
    }
};

int main()
{
    int arr[][10] =
    {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        {9, 8, 7, 5, 5, 4, 3, 2, 1, 0},
        {1, 9, 2, 7, 3, 7, 4, 6, 0, 5},
        {1, 0, 0, 2, 3, 0, 0, 0, 0, 1},
    };
    for (int j = 0; j < 4; ++j)
    {
        CHeapSort obj;
        obj.sort(arr[j], 10);
        for (int i = 0; i < 10; ++i)
        {
            printf("%d ", arr[j][i]);
        }
        printf("\n");
    }
    return 0;
}