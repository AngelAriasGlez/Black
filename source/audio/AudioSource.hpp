#ifndef _ADJ_AUDIOSOURCE_H
#define _ADJ_AUDIOSOURCE_H

#include "AudioFormat.hpp"

	class AudioSource {
	public:

		AudioSource();
		virtual ~AudioSource();
		unsigned long getLength();
		double getSamplerate();
		unsigned short getChannels();
		unsigned short getBits();

		unsigned short getFrameSize();
		unsigned short getSampleSize();

		virtual int read(Sample* pointer, int frameCount) = 0;
		virtual void seek(int frameNumber) = 0;

		virtual int getPosition();
		virtual void setPosition(int position);
	//protected:
		void setLength(unsigned long lenght);
		void setSamplerate(double samplerate);
		void setChannels(unsigned short channels);
		void setBits(unsigned short bits);

		double mPosition;

		unsigned long mLenght;
		double mSamplerate;
		unsigned short mChannels;
		unsigned short mBits;
	};

#endif



