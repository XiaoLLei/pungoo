#include <iostream>
#include <stdio.h>

using namespace std;

void printArr(int* arr, int num)
{
    for (int i = 0; i < num; ++i)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int partition(int* arr, int l, int r)
{
    int i = l;
    int j = r;
    int pivot = i;
    int number = arr[i];
    while (i < j)
    {
        while (i < j && arr[j] > number)
        {
            --j;
        }
        while (i < j && arr[i] <= number)
        {
            ++i;
        }
        if (i < j)
        {
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
        else if (i == j)
        {
            if (i != pivot)
            {
                arr[pivot] = arr[i];
                arr[i] = number;
                pivot = i;
            }
        }
        else
        {
            printf("how ? \n");
        }
    }
    return pivot;
}

void sort(int* arr, int l, int r)
{
    if (l < r)
    {
        int pivot = partition(arr, l, r);
        sort(arr, l, pivot - 1);
        sort(arr, pivot + 1, r);
    }
}

void quicksort(int* arr, int len)
{
    sort(arr, 0, len - 1);
}


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
        quicksort(arr[j], 10);
        for (int i = 0; i < 10; ++i)
        {
            printf("%d ", arr[j][i]);
        }
        printf("\n");
    }
    return 0;
}