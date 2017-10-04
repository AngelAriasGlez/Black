#include "AudioSource.hpp"

	AudioSource::AudioSource(){
		mLenght = 0;
		mSamplerate = 44100;
		mChannels = 2;
		mBits = 16;

		mPosition = 0;
	}
	AudioSource::~AudioSource(){
		
	}

	unsigned long AudioSource::getLength(){
		return mLenght;
	}
	void AudioSource::setLength(unsigned long lenght){
		mLenght = lenght;
	}
	double AudioSource::getSamplerate(){
		return mSamplerate;
	}
	void AudioSource::setSamplerate(double samplerate){
		mSamplerate = samplerate;
	}
	unsigned short AudioSource::getChannels(){
		return mChannels;
	}
	void AudioSource::setChannels(unsigned short channels){
		mChannels = channels;
	}
	unsigned short AudioSource::getBits(){
		return mBits;
	}
	void AudioSource::setBits(unsigned short bits){
		mBits = bits;
	}

	void AudioSource::setPosition(int position){
		if(position < 0){
			mPosition = 0;
		}else if(mLenght < position && position > 0){
			mPosition = mLenght;
		}else{
			mPosition = position;
		}
	}

	int AudioSource::getPosition(){
		return mPosition;
	}

	unsigned short AudioSource::getFrameSize(){
		return getSampleSize() * mChannels;
	}
	unsigned short AudioSource::getSampleSize(){
		return mBits / 8;
	}
		


