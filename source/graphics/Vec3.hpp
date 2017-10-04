#ifndef __VEC3__H
#define __VEC3__H

#include "../core/StringStream.hpp"

template<typename TX, typename TY, typename TZ>
class Vec3 {
public:
	Vec3(){};
	Vec3(TX x) :x(x){}
	Vec3(TX x, TY y) :x(x), y(y){}
	Vec3(TX x, TY y, TZ z) :x(x), y(y), z(z) {};
	TX x = 0;
	TY y = 0;
	TZ z = 0;

	Vec3 operator*(const Vec3& in) {
		return Vec3(x * in.x, y * in.y, z * in.z);
	}
	Vec3 operator/(const Vec3& in) {
		return Vec3(x / in.x, y / in.y, z / in.z);
	}
	Vec3 operator-(const Vec3& in) {
		return Vec3(x - in.x, y - in.y, z - in.z);
	}
	Vec3 operator+(const Vec3& in) {
		return Vec3(x + in.x, y + in.y, z + in.z);
	}
	Vec3 operator+=(const Vec3& in) {
		return Vec3(x + in.x, y + in.y, z + in.z);
	}
	bool operator!=(const Vec3& in) {
		return x != in.x || y != in.y || z != in.z;
	}
	bool operator==(const Vec3& in) {
		return x == in.x && y == in.y && z == in.z;
	}


	/*virtual String toString() {
		return (StringStream&)(StringStream() << x << "," << y << "," << z);
	}*/
};

#endif
