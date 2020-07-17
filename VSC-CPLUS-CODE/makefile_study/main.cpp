#include "head.h"
#include<iostream>
using namespace std;

int main()
{
	shape shape_one(3.0,4.0);
	shape shape_two(1.0,2.0);
	shape_one.get_area();
	shape_two.get_area();
	cout<<"比较两个矩形面积大小："
		<<(shape_one<shape_two)<<endl;
	triangle triangle_one(10.0,2.0);
	triangle triangle_two(3.0,4.0);
	triangle_one.output();
	shape *test_one;
	shape *test_two;
	test_one=&triangle_one;
	test_two=&triangle_two;
	test_one->get_area();
	test_two->get_area();
	cout<<"比较两个三角形面积大小: "
		<<(triangle_one<triangle_two)<<endl;
//	shape_one.get_area();
//	test_one->get_area();
//	triangle_one.output();
	return 0;
}
