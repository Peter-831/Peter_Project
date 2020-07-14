#include <iomanip>
#include <iostream>
#include <string>
using namespace std;
int main()
{
    int temp = 0;
    int array[9] = {4, 2, 8, 0, 5, 7, 1, 3, 9};
    for (int i = 0; i < 9 - 1; i++)
    {
        for (int j = 0; j < 9 - 1 - i; j++)
        {
            if (array[j] > array[j + 1])
            {
                temp = array[j + 1];
                array[j + 1] = array[j];
                array[j] = temp;
            }
        }
    }
    for (int i = 0; i < 9; i++)
        cout << array[i] << endl;

    int array1[2][3] =
        {
            {1, 2, 3},
            {4, 5, 6}};
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
            cout << array1[i][j] << " ";
        cout << endl;
    }
    //--------------成绩案例-------------
    string str1 = "语文", str2 = "数学", str3 = "英语",
           str4 = "*——*", str5 = "总分";

    float ave[3] = {0.0f, 0.0f, 0.0f},
          Total_Sum = 0.0f;

    cout << str4 << "  " << str1 << "  " << str2
         << "  " << str3 << "  " << str5 << endl;

    int grade[3][3] =
    {
        {100, 100, 100},
        {90, 50, 100},
        {60, 70, 80}
    };
    for (int i = 0; i < 3; i++)
    {
        switch (i)
        {
        case 0:
            cout << "张三 ";
            break;
        case 1:
            cout << "李四 ";
            break;
        case 2:
            cout << "王五 ";
            break;
        default:
            break;
        }
        for (int j = 0; j < 3; j++)
        {
            cout << right << setw(6) << grade[i][j];
            Total_Sum = grade[i][j];
            if(j==0)
            {
                ave[0] += grade[i][j];
            }
            else if(j==1)
            {
                ave[1] += grade[i][j];
            }   
            else if(j==2)
            {
                ave[2] += grade[i][j];
            }
        }
        cout << right << setw(6) << Total_Sum << endl;
    }
    cout << "平均分 ";
    for (int i = 0; i < 3; i++)
    {
        cout << right << setw(6) << fixed
             << setprecision(2) << ave[i] / 3;
    }

    return 0;
}
