#ifndef _BASICVIEW_H
#define _BASICVIEW_H

#include "../View.hpp"

class BasicView : public View{
public:
	BasicView() : View() {
	}
	BasicView(Rect r) : View(r) {
	}

	virtual std::string toString() {
		return "BasicView::" + View::toString();
	}

	virtual View *clone() const override { return new BasicView(*this); };
};
#endif
