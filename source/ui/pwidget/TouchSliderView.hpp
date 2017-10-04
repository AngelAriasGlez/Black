#ifndef _ADJ_TOUCHSLIDER_H
#define _ADJ_TOUCHSLIDER_H

#include "../../utils/Utils.hpp"

#include "../../graphics/Metrics.hpp"

#include <algorithm>

#include "SliderBase.hpp"
#include "../../graphics/Canvas.hpp"
#include "../../core/Timer.hpp"

class TouchSliderView : public SliderBase, Timer::ITimerListener {
public:
	enum Orientation {
		HORIZONTAL,
		VERTICAL
	};

private:
	Orientation mOrientation;

	Timer mTimer;
	int mPrevY;

public:


	TouchSliderView(Rect r) : SliderBase(r){
		mOrientation = VERTICAL;

		mTimer.setMs(100);
		mTimer.setListener(this);

	}

	void processTouch(TouchEvent e) {



		mPrevY = e.y;
		int ymax = std::max(0.0, getHeight());
		double prog = 0;
		prog += 1. - (2. / (float)ymax * (float)(e.y));

		mPrevIncrement = prog;

		if (e.type == TouchEvent::DOWN) {
			mTimer.start(true);
		}else if(e.type == TouchEvent::UP){
			mTimer.reset();
		}

	}


	void draw(Canvas *canvas){
		BasicView::draw(canvas);

		int railMargin = getWidth() / 4.5;

		int linesize = Metrics::dpToPx(2);
		int linemargin = 10;

		canvas->drawQuad(Rect(railMargin, 0, getWidth() - (railMargin * 2), getHeight()), 4, Color("#333333"));
		canvas->drawLine(Point(railMargin *1.3, getHeight() / 2), Point(getWidth() - (railMargin * 1.3), getHeight() / 2), 4, Color("#88ccdd"));


		if (isTouchDown()) {
			int y = Utils::clamp(mPrevY, linemargin, getHeight() - linemargin);
			canvas->drawQuad(Rect(railMargin * 1.3, (getHeight() / 2), getWidth() - (railMargin * 2.6), y - (getHeight() / 2)), 0, Color("#88ccdd66"));
		}

		canvas->drawFont(Point(), String(getProgress()), Font(), Color::WHITE);



		canvas->drawFont(Point(getWidth() / 2 - 20,15), L"\uF0DE", Font("fa.ttf", 80), Color("#333333"));

		canvas->drawFont(Point(getWidth() / 2 - 20, getHeight() - 100), L"\uF0DD", Font("fa.ttf", 80), Color("#333333"));

		mTimer.update();
	}


	void onTick(std::string label) {
		mProgress += mPrevIncrement;
		notifyProg();
	}

};

#endif
