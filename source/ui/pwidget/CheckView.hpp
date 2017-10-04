#ifndef _ADJ_WCHECKVIEW_H
#define _ADJ_WCHECKVIEW_H

#include "../View.hpp"
#include "BasicView.hpp"
#include "../../graphics/Canvas.hpp"

class CheckView : public BasicView, View::ITouchListener{
public:
class IChangeListener {
	public:
		virtual void onChangeListener(CheckView* self, bool selected, bool fromUser) = 0;
	};
private:
	bool mChecked;
	IChangeListener* mListener;
public:
	CheckView(int width, int height) : BasicView(width, height) {
		mChecked = false;
		addOnTouchListener(this);
	}

	void setOnChangeListener(IChangeListener* listener) {
		mListener = listener;
	}

	void setSelected(bool selected) {
		mChecked = selected;
		if (mListener) mListener->onChangeListener(this, mChecked, false);
	}

	void draw(Canvas* canvas) {
		BasicView::draw(canvas);
		canvas->drawQuad(Point(), mBounds.rect, 5, Color());
		if (mChecked) {
			Dimension in = mBounds.rect;
			in.height *= 0.8;
			in.width *= 0.8;

			canvas->drawQuad(Point(in.width * 0.12, in.height * 0.12, 1), in, 0, Color("#eeeeee"));
		}

	}


	void onClickListener(std::string label, View* view, TouchEvent& e) {
		mChecked = !mChecked;
		if (mListener) mListener->onChangeListener(this, mChecked, true);
	};



};

#endif
