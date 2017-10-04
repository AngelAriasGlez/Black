#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "../../core/Timer.hpp"
#include "../View.hpp"

class Animation : public Timer{
public:
	class IAnimationListener {
	public:
		virtual void onStart(std::string, View *) = 0;
		virtual void onEnd(std::string, View *) = 0;
		virtual void onAnimate(std::string, View *, int, int, int) = 0;
	};
protected:
	View *mView;
	IAnimationListener *mListener;
public:
	Animation(int ms = 500, View* view = NULL):
	mView(view),
	Timer(ms){
		mListener = 0;
		mView = 0;
	}

	void setView(View* view){
		mView = view;
	}
	void start(){
		if (!isRunning()){
			start(mView);
			if (mListener)
				mListener->onStart(mLabel, mView);
		}
		Timer::start();
	}
	void pause(){
		Timer::pause();
		end(mView);
		if (mListener)
			mListener->onEnd(mLabel, mView);
	}
	void tick(int elapsed, int totalElapsed){
		animate(mView, elapsed, totalElapsed, mMs);
		if (mListener)
			mListener->onAnimate(mLabel, mView, elapsed, totalElapsed, mMs);
	}
	void setListener(IAnimationListener *listener) {
		mListener = listener;
	}
protected:
	virtual void start(View *view){

	}
	virtual void end(View *view){

	}
	virtual void animate(View *view, int elapsed, int totalelapsed, int totalms){

	}
};

#endif
