#ifndef _ADJ_LINEARLAYOUT_H
#define _ADJ_LINEARLAYOUT_H

#include "../ViewGroup.hpp"

class LinearLayout : public ViewGroup{

public:

	LinearLayout() : ViewGroup(){

	}
	LinearLayout(mt::Rect r) : ViewGroup(r) {

	}
	virtual std::string toString() {
		return "LinearLayout::" + ViewGroup::toString();
	}

	virtual View *clone() const override { return new LinearLayout(*this); };
};

#endif
