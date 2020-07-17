#include<iostream>
#include <stdio.h>
using namespace std;
void output(char **Parray,int num)
{
  for (int i = 0; i<num; i++)
  {
    cout << *Parray++ << endl; 
  }
}
int main()
{
  int b[2][5] = {{1,2,3,4,5},{6, 7, 8,9,10}};//二维数组
  int(*P)[5];    //数组指针
  P = b;
  for (int i = 0; i < sizeof(b)/sizeof(b[0]);i++)  //sizeof(b)是整个二维数组元素长度
  {                                                //sizeof(b[0])是第一行所有数组元素长度
    for (int j = 0; j < sizeof(b[0])/sizeof(int);j++) //sizeof(int)是单个数组元素长度
    {
      cout << *(*(P + i) + j) << " ";
    }
    cout << endl;
  }
  cout << endl;
  char *parr[]={"王君","陈嘉炘","3","4"};
  output(parr,sizeof(parr)/sizeof(char *));
  return 0;
}
