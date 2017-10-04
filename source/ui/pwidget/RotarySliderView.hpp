#ifndef _ADJ_RSLIDERVIEW_H
#define _ADJ_RSLIDERVIEW_H


#include "../../utils/Utils.hpp"

#include "../../graphics/Metrics.hpp"

#include <algorithm>

#include "SliderBase.hpp"
#include "../../graphics/Canvas.hpp"
#include "../../core/Timer.hpp"

class RotarySliderView : public SliderBase{
public:

private:


	int mStartY;
	int mStartX;

	double mAngle;

	double mMaxAngle;
public:


	RotarySliderView(int width, int height) : SliderBase(width, height) {
		mOrigin = Origin::LEFT;

		mMaxAngle = 145;

		if (mOrigin == Origin::LEFT) mAngle = -mMaxAngle;
	}

	void processTouch(TouchEvent e) {

		if (e.type == TouchEvent::DOWN) {
			mStartY = e.y;
			mStartX = e.x;
		}else if (e.type = TouchEvent::DRAG) {
			mAngle = -(mStartY - e.y) + -(mStartX - e.x);
			if (mAngle > mMaxAngle) {
				mAngle = mMaxAngle;
			}
			else if (mAngle < -mMaxAngle) {
				mAngle = -mMaxAngle;
			}
			double prog = 50. / mMaxAngle * mAngle;
			if (mOrigin == Origin::LEFT) prog += 50;

			double diff = mProgress - prog;
			mProgress = prog;
			if (mProgressListener) {
				mProgressListener->onProgressListener(this, mProgress, diff, true);
			}
		}

	}


	void draw(Canvas *canvas) {
		View::draw(canvas);



		canvas->drawTorus(Position(), getWidth() / 2, 8, Color("#ffffff33"), -90 + -mMaxAngle, -90 + mMaxAngle, true);

		if (mOrigin == Origin::LEFT) {
			canvas->drawTorus(Position(), getWidth() / 2, 8, Color::WHITE, -90 + -mMaxAngle, mAngle - 90, true);
		}else {
			canvas->drawTorus(Position(), getWidth() / 2, 8, Color::WHITE, -90, mAngle - 90, mAngle > 0);
		}


		canvas->translateProjection(Position((getWidth()/2), (getHeight() / 2)));
		canvas->rotateProjection(mAngle, Position(0,0,1));

		canvas->drawQuad(Position(-4, 0), Dimension(8, -(getHeight() / 2)), 0, Color::WHITE);


	}



};

#endif
