#include"head.h"
#include<iostream>
using namespace std;

shape::shape(double l,double w)
{
	length=l;
	width=w;
}
void shape::get_area()
{
	area=width*length;
	cout<<"长方形面积: "<<area<<endl;
}
bool shape::operator<(const shape shape_init)
{
	if(this->area<shape_init.area)
	{
		return true;
	}
	else
		return false;
}
//----------------------------
void triangle::get_area()
{
	area=length*width/2;
	cout<<"三角形面积: "<<area<<endl;
}
void triangle::output()
{
	cout<<"this is triangle "<<endl;
}
