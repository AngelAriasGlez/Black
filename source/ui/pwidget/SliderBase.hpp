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
		LINEAR,
		LINEAR_INF,
		CIRCULAR
	};
	enum Origin {
		LEFT,
		CENTER,
		RIGHT,
		TOP,
		BOTTOM
	};
protected:
	double mPreviusAngle;

protected:
	double mProgress;
	double mPrevIncrement;

	IProgressListener *mProgressListener;
	Type mType;
	Origin mOrigin;

	//View *mRail;
public:


	SliderBase(Rect r) : BasicView(r) {
		mProgress = 0;
		mProgressListener = NULL;


		mType = LINEAR;
		mOrigin = LEFT;
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
	void setOrigin(Origin origin) {
		mOrigin = origin;
		setProgress(0);
	}
	Origin getOrigin() {
		return mOrigin;
	}


	void setProgress(double progress){
		double np = Utils::clamp(progress, -100, 100);
		mProgress = np;
		notifyProg();
	}

	int getProgress(){
		return mProgress;
	}
	

	void onDoubleClickListener(std::string label, View* view, TouchEvent& e)  {
		mProgress = 0;
		notifyProg();
	}
	void onTouchListener(std::string label, View* view, TouchEvent& e)  {
		processTouch(e);

	}

	virtual void processTouch(TouchEvent e) {
		if(e.type == TouchEvent::DOWN || (mTouchDown && e.type == TouchEvent::MOVE)){
				if (mType == CIRCULAR) {
					if (e.type == TouchEvent::DOWN) {
						return;
					}
					double angle = angleFromPoint(e.x, e.y);
					double diff = angle - mPreviusAngle;
					mPreviusAngle = angle;
					if (diff > 300) {
						diff -= 359.9;
					}
					else if (diff < -300) {
						diff += 359.9;
					}
					double diffProgress = (100. / 360. * diff);
					double progress = mProgress + diffProgress;

					mPrevIncrement = diffProgress;
					mProgress = progress;
					notifyProg();


				}else if(mType == LINEAR){

						int ymax = std::max(0.0, getHeight());
						int prog = 0;
						if (mOrigin == CENTER) {
							prog = (200. / (float)ymax * (float)(e.y)) - 100;
							prog = -Utils::clamp(prog, -100, 100);
						}
						else {
							prog = 100 - (100. / (float)ymax * (float)e.y);
							prog = Utils::clamp(prog, 0, 100);
						}
						mPrevIncrement = mProgress - prog;
						mProgress = prog;
						notifyProg();
				}
		}

	}
	void notifyProg() {
		if (mProgressListener) {
			mProgressListener->onProgressListener(this, mProgress, mPrevIncrement, true);
		}
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
