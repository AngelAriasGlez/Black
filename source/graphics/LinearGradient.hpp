#ifndef _LINEARGRAD_H
#define _LINEARGRAD_H

#include "Color.hpp"
#include "Point.hpp"

class LinearGradient {
public:
	mt::Point startPoint;
	mt::Point endPoint;
	Color startColor;
	Color endColor;
	LinearGradient() {

	}
	LinearGradient(mt::Point startPoint, mt::Point endPoint, Color startColor, Color endColor) 
	: startPoint(startPoint), endPoint(endPoint), startColor(startColor), endColor(endColor){

	}
};


#endif
