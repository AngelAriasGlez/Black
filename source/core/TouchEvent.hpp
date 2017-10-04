#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

#include <vector>
#include "../graphics/Point.hpp"
class View;

class TouchEvent {
public:
	int id;
	int x;
	int y;
	int rawX;
	int rawY;
	enum Type { DOWN, UP, MOVE, DBLCLK, LONG, CLK, DRAG, WHEEL};
	Type type;
	long time;
	bool handled;

	short wheel;
    mt::Point dragDiff;
    mt::Point dragDown;
	TouchEvent() {
		handled = false;
	}

	std::string print() {
		std::stringstream s;
		s << "id: " << id << "\n";
		s << "x: " << x << "\n";
		s << "y: " << y << "\n";
		s << "rawx: " << rawX << "\n";
		s << "rawy: " << rawY << "\n";
		s << "type:" << type << "\n";
		s << "handled:" << id << "\n";
		return s.str();
	}

};

#endif
