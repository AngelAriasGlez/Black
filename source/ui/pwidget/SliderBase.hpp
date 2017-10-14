#ifndef _ADJ_SLIDERBASE_H
#define _ADJ_SLIDERBASE_H

#include "../../utils/Utils.hpp"

#include "../../graphics/Metrics.hpp"

#include <algorithm>
#include "../../utils/MathDefs.hpp"

#include "BasicView.hpp"
#include "../../graphics/Canvas.hpp"


class SliderBase : public BasicView{
public:
	class IProgressListener{
	public:
		virtual void onProgressListener(SliderBase* slider, double progress, double diff, bool fromUser) = 0;
	};
	enum Type{
		LINEAR_HORIZONTAL_CENTER,
		LINEAR_HORIZONTAL_LEFT,
		LINEAR_VERTICAL_CENTER,
		LINEAR_VERTICAL_BOTTOM,
		LINEAR_INF,
		CIRCULAR
	};

protected:
	double mPreviusAngle;

protected:
	double mProgress;
	double mPrevIncrement;

	IProgressListener *mProgressListener;
	Type mType;

	//View *mRail;
public:


	SliderBase(Rect r) : BasicView(r) {
		mProgress = 0;
		mProgressListener = NULL;


		mType = LINEAR_VERTICAL_BOTTOM;
		mPreviusAngle = 0;
	}


	~SliderBase(){

	}
	void setType(Type type){
		mType = type;
		setProgress(0);
	}
	Type getType() {
		return mType;
	}


	void setProgress(double progress){
		double np = Utils::clamp(progress, -100, 100);
		mProgress = np;
		notifyProg();
	}

	int getProgress(){
		return mProgress;
	}



	bool onTouchEvent(TouchEvent e) override {
		if(e.type == TouchEvent::DOWN || (mTouchDown && e.type == TouchEvent::MOVE)){
			double progress;

				if (mType == CIRCULAR) {
					if (e.type == TouchEvent::DOWN) {
						return true;
					}
					double angle = angleFromPoint(e.pos.x, e.pos.y);
					double diff = angle - mPreviusAngle;
					mPreviusAngle = angle;
					if (diff > 300) {
						diff -= 359.9;
					}
					else if (diff < -300) {
						diff += 359.9;
					}
					double diffProgress = (100. / 360. * diff);
					progress = mProgress + diffProgress;

					mPrevIncrement = diffProgress;


				}else if(mType == LINEAR_VERTICAL_CENTER){
					progress = (200. / getPathSize() * (double)(e.pos.y)) - 100;
					progress = -Utils::clamp(progress, -100, 100);
					mPrevIncrement = mProgress - progress;
				}else if (mType == LINEAR_VERTICAL_BOTTOM) {
					progress = 100 - (100. / getPathSize() * (double)e.pos.y - getPathLateralMargin());
					progress = Utils::clamp(progress, 0, 100);
					mPrevIncrement = mProgress - progress;
				}else if (mType == LINEAR_HORIZONTAL_CENTER) {
					progress = (200. / getPathSize() * (double)(e.pos.x)) - 100;
					progress = -Utils::clamp(progress, -100, 100);
					mPrevIncrement = mProgress - progress;
				}else if (mType == LINEAR_HORIZONTAL_LEFT) {
					progress = (100. / getPathSize() * (double)e.pos.x - getPathLateralMargin());
					progress = Utils::clamp(progress, 0, 100);
					mPrevIncrement = mProgress - progress;
				
				}




				mProgress = progress;
				notifyProg();
		}
		return true;

	}
	void notifyProg() {
		if (mProgressListener) {
			mProgressListener->onProgressListener(this, mProgress, mPrevIncrement, true);
		}
	}

	virtual double getPathSize() {
		if (mType == LINEAR_VERTICAL_CENTER || mType == LINEAR_VERTICAL_BOTTOM) {
			return std::max(0.0, getInnerHeight());
		}
		else if (mType == LINEAR_HORIZONTAL_CENTER || mType == LINEAR_HORIZONTAL_LEFT) {
			return std::max(0.0, getInnerWidth());
		}

		return 0;
	}
	virtual double getPathLateralMargin() {
		return 0;
	}



	void setOnProgressListener(IProgressListener *listener){
		mProgressListener = listener;
	}
	virtual void draw(Canvas *canvas){
		BasicView::draw(canvas);

	}


	double angleFromPoint(int x, int y) {
		int cx = std::min(getWidth() / 2, getHeight() / 2);
		int cy = cx;

		double dy = ((getHeight() / 2) - y);
		double dx = ((getWidth() / 2) - x);


		double theta = atan2(dx, dy);

		//LOGE("%f %f %f", dx, dy, (theta * 180. / M_PI));

		return (theta * 180. / M_PI) + 180;

	}

};

#endif
