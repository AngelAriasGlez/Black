#ifndef _MOVEANIMATION_H
#define _MOVEANIMATION_H

#include "Animation.hpp"
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
	void run(mt::Point start, mt::Point end, bool hideOnFinish){
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

		view->setPosition(mStart);
	}
	void end(View* view){
		view->setPosition(mEnd);

		if (mHide){
			view->setVisibility(View::HIDDEN);
		}
	}
	void animate(View* view, int elapsed, int totalelapsed, int totalms){
		ianim(mStart, mEnd, view, totalelapsed, totalms);

	}

	void ianim(mt::Point start, mt::Point end, View* view, int totalelapsed, int totalms) {
		mt::Point res;
		if (start.x > end.x) {
			res.x = start.x - ((float)(start.x - end.x) / (float)totalms * totalelapsed);
		}
		else {
			res.x = start.x + ((float)(end.x - start.x) / (float)totalms * totalelapsed);
		}
		if (start.y > end.y) {
			res.y = start.y - ((float)(start.y - end.y) / (float)totalms * totalelapsed);
		}
		else {
			res.y = start.y + ((float)(end.y - start.y) / (float)totalms * totalelapsed);
		}

		view->setPosition(res);
	}
};

#endif