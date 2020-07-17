#ifndef head_h
#define head_h

#include<iostream>
using namespace std;
class shape
{
	public:
			shape(double l=0.0,double w=0.0);
			bool operator<(const shape shape_init);
			virtual void get_area();
	protected:
			double width;
			double length;
			double area;
};
class triangle:public shape
{
	public:
			triangle(double l=0.0,double w=0.0):shape(l,w){}
			virtual void get_area();
			void output();
};

#endif
