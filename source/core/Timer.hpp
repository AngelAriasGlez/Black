#ifndef _TIMERX_H
#define _TIMERX_H

#include <time.h>
#include <set>
#include "String.hpp"


class Timer{
public:
	static std::set<Timer*> *TIMERS;
	class ITimerListener{
	public:
		virtual void onComplete(std::string) {};
		virtual void onStart(std::string) {};
		virtual void onReset(std::string) {};
		virtual void onTick(std::string) {};
	};

protected:
	int mMs;
	long mTimePrevUpdate;
	long mTimeStart;
	bool mRunning;
	bool mIsComplete;
	bool mLoop;
	std::string mLabel;

	ITimerListener *mListener;
public:
	Timer(int ms = 500) :
		mMs(ms),
		mTimePrevUpdate(0),
		mTimeStart(0),
		mRunning(false),
		mIsComplete(false),
		mLoop(false),
		mListener(NULL){
		if (!TIMERS) {
			TIMERS = new std::set<Timer*>();
		}
		reg();
	}
	~Timer(){
		unreg();
	}
	void reg() {
		TIMERS->insert(this);
	}
	void unreg() {
		TIMERS->erase(this);
	}

	void setLabel(std::string label) {
		mLabel = label;
	}
	std::string getLabel() {
		return mLabel;
	}
	void setMs(int ms){
		mMs = ms;
	}
	int getMs(){
		return mMs;
	}
	bool isRunning(){
		return mRunning;
	}
	void reset(){
		mIsComplete = false;
		mRunning = false;
		mTimeStart = 0;
		if (mListener){
			mListener->onReset(mLabel);
		}
	}
	virtual void start(){
		start(false);
	}
	virtual void start(bool loop){
		mLoop = loop;
		mRunning = true;
		mTimeStart = ((double)clock() / (double)CLOCKS_PER_SEC) * 1000.;
		mIsComplete = false;
		if (mListener) {
			mListener->onStart(mLabel);
		}
	}

	virtual void pause(){
		mRunning = false;
		mTimeStart = 0;
	}
	bool isComplete(){
		return mIsComplete;
	}
	bool update(){
		if (mRunning){
			long currentMs = ((double)clock() / (double)CLOCKS_PER_SEC) * 1000.;
			long totalElapsed = (currentMs - mTimeStart);
			if ((currentMs - mTimeStart) < mMs){
				long elapsed = currentMs - mTimePrevUpdate;
				tick(elapsed, totalElapsed);
				mTimePrevUpdate = currentMs;
				if (mListener){
					mListener->onTick(mLabel);
				}
			}else{
				mIsComplete = true;
				if (mLoop) {
					start(true);
				}else {
					pause();
				}
				if (mListener){
					mListener->onComplete(mLabel);
				}
			}
			return true;
		}
		return false;
	}
	virtual void tick(int elapsed, int totalElapsed){
		
	}

	void setListener(ITimerListener *listener){
		mListener = listener;
	}

};

#endif
