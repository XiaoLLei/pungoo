#include <stdio.h>

class CHeapSort
{
public:
    void buildheap(int* arr, int length)
    {
        int heapsize = length;
        int nonleaf = length / 2 - 1;
        for (int i = nonleaf; i >= 0; --i)
        {
            heapify(arr, i, heapsize);
        }
    }

    void heapify(int* arr, int i, int heapsize)
    {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < heapsize)
        {
            if (arr[i] > arr[left])
            {
                smallest = left;
            }
            else smallest = i;
        }
        if (right < heapsize)
        {
            if (arr[smallest] > arr[right])
            {
                smallest = right;
            }
        }
        if (smallest != i)
        {
            int temp;
            temp = arr[i];
            arr[i] = arr[smallest];
            arr[smallest] = temp;
            heapify(arr, smallest, heapsize);
        }
    }

    void heapsort(int* arr, int length)
    {
        int heapsize = length;
        buildheap(arr, length);

        for (int i = 0; i < length - 1; ++i)
        {
            // swap the first and the last
            int temp;
            temp = arr[0];
            arr[0] = arr[heapsize - 1];
            arr[heapsize - 1] = temp;
            // heapify the arr
            heapsize = heapsize - 1;
            heapify(arr, 0, heapsize);
        }
    }
};

int main()
{
    int arr[][10] =
    {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
        {1, 9, 2, 8, 3, 7, 4, 6, 0, 5},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    };
    for (int j = 0; j < 4; ++j)
    {
        CHeapSort obj;
        obj.heapsort(arr[j], 10);
        for (int i = 0; i < 10; ++i)
        {
            printf("%d ", arr[j][i]);
        }
        printf("\n");
    }
    return 0;
}