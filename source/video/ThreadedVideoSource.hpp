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
	LaVideoSource *mSource;

	mutex mutex;
	thread thread;

	Semaphore sem;

	volatile bool needFrame = false;

	
    volatile int64_t seekPosi = -1;
	volatile bool seekPrec = true;
    
	volatile uint64_t position = 0;
    
    uint8_t* pFrame = nullptr;
    
    uint64_t lenght = 0;
    uint64_t width = 0;
    uint64_t height = 0;
    double framerate = 0;
    std::string url;
    double rotation = 0;
    unsigned short channels = 0;

	

public:
	ThreadedVideoSource() {
		thread = std::thread(&ThreadedVideoSource::work, this);
		thread.detach();

	}

	void work() {
		sem.notify();
		while (true) {
			sem.wait();
            if(!mSource) continue;
            if(seekPosi > 0){
                LOGE("seek %d", seekPosi);
                mSource->seek(seekPosi, true);
                seekPosi = -1;
            }
            if(needFrame){
                pFrame = mSource->read();
                position = mSource->getPosition();
                //LOGE("frame %d", position);
                needFrame = false;
            }
		}
	}


	
	virtual uint64_t readForward(uint8_t *frame) {
        //needFrame = true;
        pFrame = mSource->read();
        if(pFrame) memcpy(frame, pFrame, getFramePixelCount());
        position = mSource->getPosition();
        //sem.notify();
        return 1;
	};
	virtual uint64_t readBackward(uint8_t *frame) {
		/*if (mutex.try_lock()) {
			buffer.copyBackward(frame, mSource.getFramePixelCount());
			position = bufferLeftPosition + (buffer.getReadPosition() / mSource.getFramePixelCount());
			mutex.unlock();
			return 1;
		}
		sem.notify();*/
		return 0;
	};

	virtual void seek(uint64_t pos, bool precise = true) {
        //seekPosi = pos;
        mSource->seek(pos);
        position = pos;
        //sem.notify();
	}



	virtual void load(std::string infile) {
		//lock_guard<std::mutex> l(mutex);
        
        if(mSource){
            delete mSource;
        }
        mSource = new LaVideoSource();

		mSource->load(infile);
        lenght = mSource->getLength();
        width = mSource->getWidth();
        height = mSource->getHeight();
        url = mSource->getUrl();
        channels = mSource->getChannels();
        rotation = mSource->getRotation();
        framerate = mSource->getFramerate();
		
		/*int nframes = mSource.getLength();
		buffer.resize(mSource.getFramePixelCount() * nframes);

		for (int i = 0; i < nframes; i++) {
			auto ptr = mSource.read();
			if(ptr) buffer.pushRight(ptr, mSource.getFramePixelCount());
		}*/
	}
	virtual uint64_t getLength(){
		return lenght;
	}
	virtual uint64_t getPosition(){
        return (seekPosi > 0)?seekPosi:position;
	}
	virtual uint64_t getWidth(){
		return width;
	}
	virtual uint64_t getHeight(){
		return height;
	}
	virtual std::string getUrl(){
		return url;
	}

	virtual double getFramerate() {
		return framerate;
	}
	virtual unsigned short getChannels() {
		return channels;
	}
	virtual double getRotation() {
		return rotation;
	}
	virtual uint64_t getFramePixelCount() {
		return getWidth()*getHeight()*getChannels();
	}

};
#endif
