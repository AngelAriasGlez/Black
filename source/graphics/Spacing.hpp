#ifndef __SPACING__H
#define __SPACING__H

#include "Vec4.hpp"

class Spacing : public Vec4<double, double, double, double>{
public:
	using Vec4::Vec4;
	Spacing() : Vec4(){}
	Spacing(double all) : Vec4(all, all, all, all){}
	Spacing(double left_right, double top_bottom) : Vec4(left_right, top_bottom, left_right, top_bottom) {}

	double getRight() {
		return x;
	}
	double getTop() {
		return y;
	}
	double getLeft() {
		return z;
	}
	double getBottom() {
		return w;
	}

	void setRight(double r) {
		x = r;
	}
	void setTop(double t) {
		y = t;
	}
	void setLeft(double l) {
		z = l;
	}
	void setBottom(double b) {
		w = b;
	}

};

#endif

