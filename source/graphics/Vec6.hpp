#ifndef __VEC6__H
#define __VEC6__H

#include "Vec4.hpp"

template<typename TX, typename TY, typename TZ, typename TW, typename TA, typename TB>
class Vec6 : public Vec4{
public:
	using Vec4::Vec4;
	Vec6();
	Vec6(TX x, TY y, TZ z, TW w, TA a) : Vec4(x, y, z, w), a(a){}
	Vec6(TX x, TY y, TZ z, TW w, TA a, TB b) : Vec4(x, y, z, w), a(a), b(b) {}
	TA a = 0;
	TB b = 0;

	/*Vec6 operator-(const Vec6& in) {
		return Vec6(x - in.x, y - in.y, z - in.z, w - in.w, a - in.a, b - in.b);
	}
	Vec6 operator+(const Vec6& in) {
		return Vec6(x + in.x, y + in.y, z + in.z, w + in.w, a + in.a, b + in.b);
	}
	Vec6 operator+=(const Vec6& in) {
		return Vec6(x + in.x, y + in.y, z + in.z, w + in.w, a + in.a, b + in.b);
	}
	Vec6 operator*(const Vec6& in) {
		return Vec6(x * in.x, y * in.y, z * in.z, w * in.w, a * in.a, b * in.b);
	}
	Vec6 operator/(const Vec6& in) {
		return Vec6(x / in.x, y / in.y, z / in.z, w / in.w, a * in.a, b * in.b);
	}
	bool operator!=(const Vec6& in) {
		return x != in.x || y != in.y || z != in.z || w != in.w || a != in.a || b != in.b;
	}
	bool operator==(const Vec6& in) {
		return x == in.x && y == in.y && z == in.z && w == in.w && a == in.a && b == in.b;
	}*/

	virtual String toString() override{
		return (StringStream&)(StringStream() << x << "," << y << "," << z << "," << w << "," << a << "," << b);
	}

};

#endif
