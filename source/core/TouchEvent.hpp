#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

#include <vector>
#include "../graphics/Point.hpp"
class View;

class TouchEvent {
public:
	int id;
	mt::Point pos;
	mt::Point rawPos;

	enum Type { DOWN, UP, MOVE, DBLCLK, LONG, CLK, DRAG, WHEEL, PINCH, PINCH_START, PAN};
	Type type;
	long time;
	bool handled;

    float data;
    mt::Point dragDiff;
    mt::Point dragDown;
    
    mt::Point gestureDown1;
    mt::Point gestureDown2;
    
	TouchEvent() {
		handled = false;
	}

	std::string print() {
		std::stringstream s;
		s << "id: " << id << "\n";
		s << "x: " << pos.x << "\n";
		s << "y: " << pos.y << "\n";
		s << "rawx: " << rawPos.x << "\n";
		s << "rawy: " << rawPos.y << "\n";
		s << "type:" << type << "\n";
		s << "handled:" << id << "\n";
		return s.str();
	}

};

#endif
