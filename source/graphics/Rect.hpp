#ifndef __RECT__H
#define __RECT__H

#include "Vec4.hpp"
#include "../utils/Utils.hpp"
namespace mt{
class Rect : public Vec4<double, double, double, double>{
public:
	using Vec4::Vec4;

	//Rect(int x, int y, int w, int h) : Vec4(x, y, w, h) {}
	//Rect(int w, int h) : Vec4(0, 0, w, h){}

	double getHeight() {

		return w;
	}
	double getWidth() {
		return z;
	}

	void setHeight(double h) {
		w = h;
	}
	void setWidth(double w) {
		z = w;
	}
	void setX(double x) {
		Vec4::x = x;
	}
	void setY(double y) {
		Vec4::y = y;
	}

	double getRight() {
		return x + z;
	}
	double getTop() {
		return y;
	}
	double getLeft() {
		return x;
	}
	double getY() {
		return y;
	}
	double getX() {
		return x;
	}
	double getBottom() {
		return y + z;
	}
	double getHorizontalMiddle() {
		return x + (z / 2);
	}
	double getVerticalMiddle() {
		return y + (w / 2);
	}

	/*
	Rect fitCenter(Rect r) {
		return Rect(r.x + Utils::max(0, (width - r.width) / 2), r.y + Utils::max(0, (height - r.height) / 2), Utils::max(width, r.width), Utils::max(height, r.height));
	}*/
};

}
#endif
