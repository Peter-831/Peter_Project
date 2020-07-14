#include<stdio.h>
#include<iostream>
#include<math.h>
using namespace std;
long long int count = 0;   //顺序对个数
long long int countrev = 0;  //逆序对个数
//end=years-1    start=0
void mergesort(int start,int end,int arr[])
{
    int temp[end - start + 1];
    if(start==end)
        return;
    int mid = (start + end) / 2;
    mergesort(start, mid, arr);
    mergesort(mid + 1, end, arr);
    int s = start;
    int m = mid + 1;
    int i = 0;    //用于temp[i]
    while(s<=mid&&m<=end)  //序列从大到小排列
    {
        if(arr[s]<arr[m])
        {
            temp[i++] = arr[m++];
            count = count + mid - s + 1;
            
        }
        else
        {
            temp[i++] = arr[s++];
        }
    }
    while(s<=mid)   //左序列有剩余
    {
        temp[i++] = arr[s++];
    }
    while(m<=end)   //右序列有剩余
    {
        temp[i++] = arr[m++];
    }
    for (int i = start; i <= end; i++)  //记得把temp[]暂存的数值重新赋值给arr[]这样新的arr就是有序的，用于递归合并
    {
        arr[i] = temp[i-start];
    }
}


int main()
{
    int d = pow(10, 7) + 9;
    int years = 200000;
    int arr[200000] = {0};
    while(cin>>years)
    {
        for (int i = 0; i < years;i++)
        {
            cin >> arr[i];
        }
        mergesort(0, years - 1, arr);
        cout << count % d << endl;
        count = 0;
    }
    return 0;
}