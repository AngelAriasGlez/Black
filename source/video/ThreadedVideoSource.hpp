#ifndef __THVIDEOSOURCE_H
#define __THVIDEOSOURCE_H

#include <string>
#include <thread>
#include <mutex>
using namespace std;

#include "VideoSource.hpp"
#include "LaVideoSource.hpp"

#include "thread/Semaphore.hpp"

#include "utils/BidirectionalBuffer.hpp"

class ThreadedVideoSource{
	LaVideoSource mSource;

	mutex mutex;
	thread thread;

	Semaphore sem;

	volatile bool needFrame = false;

	volatile bool needSeek = false;
	volatile uint64_t seekPosi = 0;
	volatile bool seekPrec = true;

	volatile uint64_t bufferLeftPosition = 0;
	volatile uint64_t position = 0;

	BidirectionalBuffer<uint8_t> buffer;

public:
	ThreadedVideoSource() {
		thread = std::thread(&ThreadedVideoSource::work, this);
		thread.detach();

	}

	void work() {
		sem.notify();
		while (true) {
			sem.wait();
			mutex.lock();
			auto ptr = mSource.read();
			if(ptr) buffer.pushRight(ptr, mSource.getFramePixelCount());
			mutex.unlock();

		}
	}


	
	virtual uint64_t readForward(uint8_t *frame) {
		if (mutex.try_lock()) {
			buffer.copyForward(frame, mSource.getFramePixelCount());
			position = bufferLeftPosition + (buffer.getReadPosition() / mSource.getFramePixelCount());
			mutex.unlock();
			return 1;
		}
		sem.notify();
		return 0;
	};
	virtual uint64_t readBackward(uint8_t *frame) {
		if (mutex.try_lock()) {
			buffer.copyBackward(frame, mSource.getFramePixelCount());
			position = bufferLeftPosition + (buffer.getReadPosition() / mSource.getFramePixelCount());
			mutex.unlock();
			return 1;
		}
		sem.notify();
		return 0;
	};

	virtual void seek(uint64_t pos, bool precise = true) {
		buffer.setReadPosition((pos*mSource.getFramePixelCount()) - bufferLeftPosition);

	}


	virtual void load(std::string infile) {
		lock_guard<std::mutex> l(mutex);
		mSource.load(infile);
		
		int nframes = mSource.getLength();
		buffer.resize(mSource.getFramePixelCount() * nframes);

		for (int i = 0; i < nframes; i++) {
			auto ptr = mSource.read();
			if(ptr) buffer.pushRight(ptr, mSource.getFramePixelCount());
		}
	}
	virtual uint64_t getLength(){
		//lock_guard<std::mutex> l(mutex);
		return mSource.getLength();
	}
	virtual uint64_t getPosition(){
		return position;
	}
	virtual uint64_t getWidth(){
		//lock_guard<std::mutex> l(mutex);
		return mSource.getWidth();
	}
	virtual uint64_t getHeight(){
		//lock_guard<std::mutex> l(mutex);
		return mSource.getHeight();
	}
	virtual std::string getUrl(){
		//lock_guard<std::mutex> l(mutex);
		return mSource.getUrl();
	}

	virtual double getFramerate() {
		//lock_guard<std::mutex> l(mutex);
		return mSource.getFramerate();
	}
	virtual unsigned short getChannels() {
		//lock_guard<std::mutex> l(mutex);
		return mSource.getChannels();
	}
	virtual double getRotation() {
		//lock_guard<std::mutex> l(mutex);
		return mSource.getRotation();
	}
	virtual uint64_t getFramePixelCount() {
		return getWidth()*getHeight()*getChannels();
	}

};
#endif
