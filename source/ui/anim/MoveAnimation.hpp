#ifndef _MOVEANIMATION_H
#define _MOVEANIMATION_H

#include "Animation.hpp"
#include "graphics/Bounds.hpp"
class MoveAnimation : public Animation{
private:
	Point mStart;
	Point mEnd;
	bool mHide;
public:
	MoveAnimation() :
		mHide(false),
		Animation(){

	}
	void run(Point start, Point end, bool hideOnFinish){
		mStart = start;
		mEnd = end;
		mHide = hideOnFinish;
		Animation::start();
	}
protected:
	void start(View* view){
		/*mStartX = view->getLeft();
		mEndX = Platform::getDisplayWidth();
		mEndY = mStartY = view->getTop();*/
		if (!view->isVisible()){
			view->setVisibility(View::VISIBLE);
		}
		Bounds b = view->getBounds();
		b.rect = mStart;
		view->setBounds(b);
	}
	void end(View* view){
		Bounds b = view->getBounds();
		b.rect = mEnd;
		view->setBounds(b);

		if (mHide){
			view->setVisibility(View::HIDDEN);
		}
	}
	void animate(View* view, int elapsed, int totalelapsed, int totalms){
		Position res;
		if (mStart.x > mEnd.x){
			res.x = mStart.x - ((float)(mStart.x - mEnd.x) / (float)totalms * totalelapsed);
		}else{
			res.x = mStart.x + ((float)(mEnd.x - mStart.x) / (float)totalms * totalelapsed);
		}
		if (mStart.y > mEnd.y){
			res.y = mStart.y - ((float)(mStart.y - mEnd.y) / (float)totalms * totalelapsed);
		}else{
			res.y = mStart.y + ((float)(mEnd.y - mStart.y) / (float)totalms * totalelapsed);
		}

		//LOGE("%d %d", left, top);
		Bounds b = view->getBounds();
		b.position = res;
		view->setBounds(b);


	}
};

#endif