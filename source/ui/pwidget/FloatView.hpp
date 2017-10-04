#ifndef _FLOATVIEW_H
#define _FLOATVIEW_H

#include "View.h"
#include "../platform/Application.h"

class FloatView : public View{
public:
	FloatView() : View_CRTP() {
	}
	FloatView(int width, int height) : View_CRTP(width, height) {

	}
	void attach() {
		Page* page = Application::getInstance()->getPageController()->getCurrentPage();
		if (page) {
			page->addView(this);
		}
	}




};
#endif
