#ifndef __VEC4__H
#define __VEC4__H

#include "Vec3.hpp"
#include "../core/StringStream.hpp"
template<typename TX, typename TY, typename TZ, typename TW>
class Vec4{
public:
    TX x = 0;
    TY y = 0;
    TZ z = 0;
	TW w = 0;

	Vec4() {
    
    }
	Vec4(TX x, TY y, TZ z, TW w) : x(x), y(y), z(z), w(w){}

/*
	Vec4 operator-(const Vec4& in){
        return Vec4(x - in.x, y - in.y, z - in.z, w - in.w);
	}
	Vec4 operator+(const Vec4& in){
		return Vec4(x + in.x, y + in.y, z + in.z, w + in.w);
	}
	Vec4 operator+=(const Vec4& in){
		return Vec4(x + in.x, y + in.y, z + in.z, w + in.w);
	}
	Vec4 operator*(const Vec4& in) {
		return Vec4(x * in.x, y * in.y, z * in.z, w * in.w);
	}
	Vec4 operator/(const Vec4& in) {
		return Vec4(x / in.x, y / in.y, z / in.z, w / in.w);
	}
	bool operator!=(const Vec4& in) {
		return x != in.x || y != in.y || z != in.z || w != in.w;
	}
	bool operator==(const Vec4& in) {
		return x == in.x && y == in.y && z == in.z && w == in.w;
	}


	virtual String toString() override{
		return (StringStream&)(StringStream() << x << "," << y << "," << z << "," << w);
	}*/

};

#endif
