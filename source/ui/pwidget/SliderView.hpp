#ifndef _ADJ_SLIDER_H
#define _ADJ_SLIDER_H

#include "../../utils/Utils.hpp"

#include "../../graphics/Metrics.hpp"

#include <algorithm>

#include "SliderBase.hpp"
#include "../../graphics/Canvas.hpp"


class SliderView : public BasicView{
protected:

	int mThumbSize;

	int mProgress = 0;
	int mPrevIncrement = 0;

	int mMax = 100;
	int mStep = 10;

	std::function<void(SliderView*, int, int)> mCallback;
public:


	void setProgress(int progress) {
		int res = progress % mStep;
		mProgress = progress - res;
		if (res > mStep / 2) mProgress += mStep;

		notifyProgress();
	}

	double getProgress() {
		return mProgress;
	}

	void setOnProgressListener(std::function<void(SliderView*, int, int)> callback) {
		mCallback = callback;
	}

	SliderView(Rect r) : BasicView(r){

	}

	double getMax() {
		return mMax;
	}


	void draw(Canvas *canvas){
		BasicView::draw(canvas);



			int railPadding = getInnerHeight() / 3;

			mThumbSize = getInnerHeight();


			canvas->drawRoundQuad(Rect(getPathX(), railPadding, getPathSize(), getInnerHeight() - (railPadding * 2)), 4, 4, Color("#fff5"));
			int x = (getPathSize() / mMax * mProgress) + getPathX();
			canvas->drawCircle(Point(x, (getInnerHeight()/2)), mThumbSize, 0, Color("#ffffff"));
				



	}
	double getPathSize() {
		return getInnerWidth() - mThumbSize;
	}
	double getPathX() {
		return mThumbSize / 2;
	}

	void notifyProgress() {
		mCallback(this, mProgress, mPrevIncrement);
	}

	bool onTouchEvent(TouchEvent e) override {
		if (e.type == TouchEvent::DOWN || e.type == TouchEvent::DRAG) {
			int progress;
			progress = (mMax / getPathSize() * (e.x - getPathX()));
			progress = Utils::clamp(progress, 0, mMax);
			mPrevIncrement = mProgress - progress;

			setProgress(progress);
		}
		return true;

	}



};

#endif
