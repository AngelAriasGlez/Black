#ifndef _POINT_H
#define _POINT_H

#include "../core/StringStream.hpp"
#include "Vec3.hpp"

namespace mt{
class Point : public Vec3<double, double, double>{
public:
	using Vec3::Vec3;
};
}

#endif
