#ifndef __VIDEOCACHE_H
#define __VIDEOCACHE_H


#include "utils/BidirectionalBuffer.hpp"
#include "VideoSource.hpp"

class VideoCached : public VideoSource{
public:

	VideoCached(){

	}
	~VideoCached() {

	}
	void load(std::string file) override{
		VideoSource::load(file);

		mBuffer.resize(framesToSamples(mReadFrames * 2));
		mBuffer.clear();
		mForwardPosition = 0;
	}

	int getPosition() override{
		return mForwardPosition - samplesToFrames(mBuffer.avaliableForward());
	}

	void seek(int64_t frameNumber, bool acurate = true) override{
		if (frameNumber <= mForwardPosition
			&& frameNumber >= (mForwardPosition - samplesToFrames(mBuffer.getElements()))) {
			int samples = framesToSamples(mForwardPosition - frameNumber);
			mBuffer.setReadPosition(mBuffer.getElements() - samples);
		}
		else {
			VideoSource::seek(frameNumber);
			mBuffer.clear();
			mForwardPosition = frameNumber;
		}
	}
	int read(uint8_t* &pointer, int frameCount) override {

		int readChunkSize = framesToSamples(mReadFrames);
		if (frameCount > 0) {
			size_t saval = mBuffer.avaliableForward();
			if (saval < readChunkSize) {
				if (VideoSource::getPosition() != mForwardPosition) {
					VideoSource::seek(mForwardPosition);
				}
				for (int i = 0; i < mReadFrames; i++) {
					int sreaded = 1;
					VideoSource::grabFrame();
					mBuffer.pushRight(VideoSource::scale(), framesToSamples(sreaded));
				}
				mForwardPosition = VideoSource::getPosition();
			}
			pointer = &mBuffer[mBuffer.getReadPosition()];
			//int breaded = mBuffer.readForward(pointer, framesToSamples(frameCount));

			//return samplesToFrames(breaded);
			return 1;
		}
		else {
			/*size_t saval = mBuffer.avaliableBackward();
			if (saval < mReadChunkSize) {
				double backwardPosition = mForwardPosition - samplesToFrames(mBuffer.getElements());

				int toread = samplesToFrames(mReadChunkSize);
				double seek = backwardPosition - toread;
				if (seek > -toread) {
					VideoSource::seek(std::max(0., seek));
					if (seek < 0) {
						toread += seek;
					}
					int sreaded = abs(VideoSource::read(mBridgeBuffer, toread));
					int scroped = mBuffer.pushLeft(mBridgeBuffer, framesToSamples(sreaded));
					mForwardPosition -= samplesToFrames(scroped);
				}
			}
			int breaded = mBuffer.readBackward(pointer, framesToSamples(abs(frameCount)));


			uint8_t swap;
			int s = breaded;
			for (int a = 0; a < --s; a++) {
				swap = ((uint8_t *)pointer)[a];
				((uint8_t *)pointer)[a] = ((uint8_t *)pointer)[s];
				((uint8_t *)pointer)[s] = swap;
			}

			return samplesToFrames(breaded) * -1;*/

		}


	}

protected:


private:
	BidirectionalBuffer<uint8_t> mBuffer;


	int mReadFrames = 100;

	double mForwardPosition = 0;

	int samplesToFrames(int samples) {
		return samples / getWidth() / getHeight() / getChannels();
	}


	int framesToSamples(int frames) {
		return frames * getWidth() * getHeight() * getChannels();
	}
};

#endif