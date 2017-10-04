#ifndef _SIZEANIMATION_H
#define _SIZEANIMATION_H

#include "Animation.hpp"

class SizeAnimation : public Animation{
private:
	bool mHide;
	int mEndWidth;
	int mEndHeight;
	int mStartWidth;
	int mStartHeight;
public:
	SizeAnimation() :
		mEndWidth(0),
		mHide(false),
		mEndHeight(0),
		Animation(){

	}
	void start(int endWidth, int endHeight, bool hideOnFinish){
		mEndWidth = endWidth;
		mEndHeight = endHeight;
		mHide = hideOnFinish;
		mStartWidth = mView->getWidth();
		mStartHeight = mView->getHeight();

		Animation::start();
	}
	void run(int startWidth, int startHeight, int endWidth, int endHeight, bool hideOnFinish){
		mEndWidth = endWidth;
		mEndHeight = endHeight;
		mStartWidth = endWidth;
		mStartHeight = endHeight;
		mHide = hideOnFinish;
		Animation::start();
	}
protected:
	void start(View* view){
		if (!view->isVisible()){
			view->setVisibility(View::VISIBLE);
		}
		view->setHeightMode(View::SizeMode::FIXED);
		view->setWidthMode(View::SizeMode::FIXED);
	}
	void end(View* view){
		view->setSize(mEndWidth, mEndHeight);
		if (mHide){
			view->setVisibility(View::HIDDEN);
		}
		view->computeParent();
	}
	void animate(View* view, int elapsed, int totalelapsed, int totalms){
		int w = mStartWidth, h = mStartHeight;
		if (mEndWidth != View::FILL && mEndWidth != View::WRAP) {
			if (mStartWidth > mEndWidth) {
				w -= ((float)(mStartWidth - mEndWidth) / (float)totalms * totalelapsed);
			}
			else {
				w += ((float)(mEndWidth - mStartWidth) / (float)totalms * totalelapsed);
			}
		}
		if (mEndHeight != View::FILL && mEndHeight != View::WRAP) {
			if (mStartHeight > mEndHeight) {
				h -= ((float)(mStartHeight - mEndHeight) / (float)totalms * totalelapsed);
			}
			else {
				h += ((float)(mEndHeight - mStartHeight) / (float)totalms * totalelapsed);
			}
		}
		//LOGE("%d", h);

		view->setSize(w, h);
		view->computeParent();

	}
};

#endif