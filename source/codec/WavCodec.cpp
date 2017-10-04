#include "WavCodec.hpp"
#include <algorithm>


	WavCodec::WavCodec(){
		mHeaderSize = 0;
		mFile = NULL;
	}
	WavCodec::~WavCodec(){
		if(mFile)
		delete mFile;
	}
	bool WavCodec::_open() {
		mFile = new FileStream(getUri());
		if (mFile->isOpen()) {
			Riff riff;
			Wave wave;

			mFile->read((void *)(&riff), sizeof(Riff));
		head:
			int sb = sizeof(wave.WAVE) + sizeof(wave.fmt) + sizeof(wave.Subchunk1Size);
			mFile->read((void *)(&wave), sb);
			//Skip Broadcast Wave
			if (memcmp(wave.fmt, "bext", 4) == 0) {
				mFile->seek(mFile->position() + wave.Subchunk1Size);
				mFile->read((void *)(&wave.fmt), sb - sizeof(wave.WAVE));
			}

			mFile->read((void *)(&wave.AudioFormat), std::min(40, (int)wave.Subchunk1Size));

			if (memcmp(riff.RIFF, "RIFF", 4) != 0 ||
				riff.ChunkSize == 0 ||
				memcmp(wave.WAVE, "WAVE", 4) != 0 ||
				(wave.AudioFormat != 0x0001 && wave.AudioFormat != 0x0003))
				return false;

			Fact fact;
			mFile->read((void *)(&fact), sizeof(Fact));
			//Skip fact
			if (memcmp(riff.RIFF, "fact", 4) != 0) {
				mFile->seek(mFile->position() - sizeof(Fact));
			}

			Data data;
		skip:
			mFile->read((void *)(&data), sizeof(Data));
			if (memcmp(data.Subchunk2ID, "data", 4)) {
				mFile->seek(mFile->position() + data.Subchunk2Size);
				if (mFile->position() >= mFile->getSize()) {
					return false;
				}
				goto skip;
			}

			mLenght = data.Subchunk2Size / (wave.bitsPerSample / 8) / wave.NumOfChan;
			mSamplerate = wave.SamplesPerSec;
			mChannels = wave.NumOfChan;
			mBits = wave.bitsPerSample;

			mHeaderSize = mFile->position();

			return true;
		}
		return false;

	}


	int WavCodec::_read(void* pointer, int frameCount){

		int readed = 0;
		if(mFile->isOpen()){
			if((mFile->position() + (frameCount * getFrameSize())) < mHeaderSize){
				frameCount = (mFile->position() - mHeaderSize) / getFrameSize();
			}
			readed = mFile->read(pointer, abs(frameCount) * getFrameSize(), frameCount < 0) / getFrameSize();
		}


		return readed;
	}

	void WavCodec::_seek(int frameNumber){
		mFile->seek(mHeaderSize + (frameNumber * getFrameSize()));

	}

