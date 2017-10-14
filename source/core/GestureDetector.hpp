#ifndef _GESTUREDETECTOR_H
#define _GESTUREDETECTOR_H

#include "Timer.hpp"
#include "TouchEvent.hpp"
#include "ListenerRegister.hpp"
#include "../graphics/Metrics.hpp"

class IGestureListener {
	public:
		virtual void onGesture(TouchEvent) {};
};



class GestureDetector : public ListenerRegister<IGestureListener>, Timer::ITimerListener {
private:
	Timer mLongTimer;
	Timer mDblTimer;

	TouchEvent mDown;
	bool isDown=false;
	bool isDrag = false;
	int mError;
public:


	GestureDetector() {
		mLongTimer.setMs(250);
		mLongTimer.setListener(this);
		//mLongTimer.reg();

		mDblTimer.setMs(250);
		//mDblTimer.reg();

        mError = Metrics::px(4);
	}
	~GestureDetector() {

	}

	void processEvent(TouchEvent e) {
		if (e.type == TouchEvent::DOWN) {
			if (checkNotMove(e) && !mDblTimer.isComplete() && mDblTimer.isRunning()) {
				notify(e);
				e.type = TouchEvent::DBLCLK;
				mDblTimer.reset();
			}else {
				mDblTimer.start();
				mLongTimer.start();
			}
			mDown = e;
			isDown = true;
		}else if (e.type == TouchEvent::UP) {
			if (checkNotMove(e)) {
				e.type = TouchEvent::CLK;
				notify(e);
			}
			e.type = TouchEvent::UP;
			mLongTimer.reset();
			isDown = false;
			isDrag = false;
		}else if (e.type == TouchEvent::MOVE) {
			notify(e);
			if ((!checkNotMove(e) || isDrag) && isDown) {
				isDrag = true;
				e.dragDiff.x = mDown.rawPos.x - e.rawPos.x;
				e.dragDiff.y = mDown.rawPos.y - e.rawPos.y;
                e.dragDown = mt::Point(mDown.rawPos.x, mDown.rawPos.y);
				e.type = TouchEvent::DRAG;
			}
		}
		notify(e);

	}

	bool checkNotMove(TouchEvent e) {
        //LOGE("%d %d", mDown.rawX - e.rawX, mDown.rawY - e.rawY);
		return abs(mDown.rawPos.x - e.rawPos.x) <= mError && abs(mDown.rawPos.y - e.rawPos.y) <= mError;

		
	}

	void onComplete(std::string label) {
		if (checkNotMove(mDown)) {
			mDown.type = TouchEvent::LONG;
			notify(mDown);
		}

	}

	void notify(TouchEvent e) {
		for (ListenerIterator i = getListeners().begin(); i != getListeners().end(); ++i) {
			(*i)->onGesture(e);
		}
	}
};
#endif
