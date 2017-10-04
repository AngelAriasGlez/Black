#ifndef _ADJ_ABSOLUTELAYOUT_H
#define _ADJ_ABSOLUTELAYOUT_H

#include "../ViewGroup.hpp"

class AbsoluteLayout : public ViewGroup{

public:
	using ViewGroup::ViewGroup;

	virtual std::string toString() {
		return "AbsoluteLayout::" + ViewGroup::toString();
	}


};

#endif
