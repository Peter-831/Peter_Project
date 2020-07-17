#include <ctime>
#include <iostream>
using namespace std;
int main()
{
    cout << "欢迎来到猜数字世界" << endl;
    cout << "游戏规则：每次随机产生1~100中的随机数一个，有六次机会猜测该随机数，猜测低于或高于该随机数系统有提示"
         << endl;
    srand((unsigned int)time(NULL));
    //int num = rand() % 100 + 1; //1~100的随机数
    int val = 0;       //所猜测的数字
    int count = 1;     //用于计数6次
    bool flag = false; //验证是否猜中标志位
    int series = 0;    //连续猜中计数
    while (1)
    {
        int num = rand() % 100 + 1; //1~100的随机数
                                    //cout <<"随机数为："<< num << endl;
        cout << "----------*************------------" << endl;
        cout << "进行第【" << count << "】次猜数字游戏,共6次机会" << endl;
        for (int i = 0; i <= 5; i++)
        {
            cout << "请输入猜测数字，范围1~100" << endl;
            cin >> val;
            flag = false;
            if (val > num)
                cout << "猜测过大,你还有" << 5 - i << "次机会" << endl;
            else if (val < num)
                cout << "猜测过小,你还有" << 5 - i << "次机会" << endl;
            else if (val == num)
            {
                cout << "猜测正确,准备开始下一场" << endl
                     << endl;
                series += 1;
                flag = true;
                break;
            }
        }
        if (!flag)
        {
            cout << endl
                 << "6次回答错误，准备进行下一场" << endl
                 << endl;
            series = 0;
        }
        if (series >= 3)
        {
            switch (series)
            {
            case 3:
                cout << "已连续正确3把" << endl;
                break;
            case 4:
                cout << "已连续正确4把" << endl;
                break;
            case 5:
                cout << "已连续正确5把" << endl;
                break;
            default:
                cout << "你已经超神了" << endl;
                series = 0;
                break;
            }
        }
        count++;
    }
    return 0;
}