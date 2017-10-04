#ifndef _ADJ_FLOATINGVIEW_H
#define _ADJ_FLOATINGVIEW_H

#include "ViewGroup.h"

class FloatingView : public ViewGroup {

public:
	FloatingView() : ViewGroup() {
		initialize();
	}
	FloatingView(int width, int height) : ViewGroup(width, height) {
		initialize();
	}

	void initialize() {
		//setBackground();
	}

	void onInitialize() {

	}
	void setPosition(int l, int t) {
		// do nothing
	}
	void hide() {
		setVisibility(HIDDEN);
	}
	void show() {
		setVisibility(VISIBLE);
	}
	inline void draw(Canvas *canvas) {

	}
};

#endif

