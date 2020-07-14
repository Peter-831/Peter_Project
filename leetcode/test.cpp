#include<stdio.h>
#include<iostream>
#include<math.h>
using namespace std;
int main()
{
    int years = 200000;
    int d = pow(10, 7) + 9;
    int arr[200000]={0};
    while (cin >> years)
    {
        int sum = 0;         
        for (int k = 0; k < years; k++)
        {
            cin >> arr[k];
        }
        for (int i = 0; i < years - 1; i++)
        {
            for (int j = 1; j < years -i; j++)
            {
                if (arr[i] < arr[i + j])
                    sum++;
            }
        }
        cout << sum%d << endl;
}
    return 0;
}