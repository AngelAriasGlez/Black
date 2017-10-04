#include "Mp3Codec.hpp"



	Mp3Codec::Mp3Codec(){

		mpauOutputBuffer = new unsigned char[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
		mpauInputBuffer = new unsigned char[MPAUDEC_MAX_AUDIO_FRAME_SIZE];

		mBuffer = new  QueueBuffer();

		mEof = false;
		mInputPosition = 0;
		mInputLength = 0;
		mPosition = 0;

		skipFrames = 0;

		mFile = NULL;
	}
	Mp3Codec::~Mp3Codec(){
		mpaudec_clear(mpauContext);
		delete mpauContext;

		delete mpauOutputBuffer;
		delete mpauInputBuffer;

		delete mBuffer;

		if (mFile)
			delete mFile;
	}

	bool Mp3Codec::_open(){
		mFile = new FileMap(getUri());
		if(mFile->isOpen()){

			mpauContext = new MPAuDecContext;
			if (mpaudec_init(mpauContext) < 0) {
				delete mpauContext;
				mpauContext = 0;
				return false;
			}
			mpauContext->parse_only = 1;
			int read;
			if(!decodeFrame())
				return false;

			mChannels = mpauContext->channels;
			mSamplerate = mpauContext->sample_rate;
			mBits = 16;

			while (!mEof) {
				if (!decodeFrame())
					return false;
				if (!mEof)
					mFrameSizes.push_back(mpauContext->frame_size);
				int frame_offset = mFile->position() -
					(mInputLength - mInputPosition) -
					mpauContext->coded_frame_size;
				mFrameOffsets.push_back(frame_offset);
				mLenght += mpauContext->frame_size;
			}

			reset();
			return true;
		}
		return false;
	}


	void Mp3Codec::_seek(int frameNumber){
		int frameSize = 0;
		int scanPosition = 0;
		int targetFrame = 0;
		int frameSizeCount = mFrameSizes.size();
		while (targetFrame < frameSizeCount) {
			frameSize = mFrameSizes[targetFrame];
			if (frameNumber <= scanPosition + frameSize)
				break;
			else {
				scanPosition += frameSize;
				targetFrame++;
			}
		}

		const int MAX_FRAME_DEPENDENCY = 10;
		targetFrame = std::max(0, targetFrame - MAX_FRAME_DEPENDENCY);
		//reset();
		mFile->seek(mFrameOffsets[targetFrame]);

		mInputPosition = 0;
		mInputLength = 0;
		mBuffer->clear();
		
		skipFrames = frameNumber - (scanPosition);

	}
	int Mp3Codec::_read(void* pointer, int frameCount){

		int toRead = abs(frameCount);
		if(frameCount < 0){
			_seek(getPosition()-toRead);
		}

		const int frameSize = getFrameSize();
		int framesReaded = 0;
		while (framesReaded < toRead) {

			// no more samples?  ask the MP3 for more
			if (mBuffer->getSize() < frameSize) {
				if (!decodeFrame() || mEof) {
					// done decoding?
					return framesReaded;
				}

				// if the buffer is still empty, we are done
				/*if (mBuffer->getSize() < frameSize) {
					return framesReaded;
				}*/
			}
			if(skipFrames > 0){
				mBuffer->ltrim(skipFrames);
				skipFrames = 0;
			}
			int readed = mBuffer->read(pointer, std::min((double)((toRead - framesReaded) * frameSize), (double)mBuffer->getSize()));
			pointer = (short*)pointer + (readed / sizeof(short));
			framesReaded += readed / frameSize;

		}
		if(frameCount < 0){
			return framesReaded*-1;
		}else{
			return framesReaded;
		}


	}
	static const int ID3v2_HEADER_SIZE = 10;

	bool Mp3Codec::decodeFrame() {

		int startPos = mFile->position();

		int ip = 0;
		int outputSize = 0;
		while (outputSize == 0) {
			if (mInputPosition == mInputLength) {
				mInputPosition = 0;
				mInputLength = mFile->read((void *)mpauInputBuffer, MPAUDEC_MAX_AUDIO_FRAME_SIZE);
				//LOGE("%d", mInputLength);
				if (mInputLength == 0) {
					mEof = true;
					return true;
				}
			}
			if(mInputLength < mInputPosition){
				mInputLength = mInputPosition;
				return true;
			}

			/* skip all ID3v2 tags */
			if (mInputLength - mInputPosition > ID3v2_HEADER_SIZE &&
				ID3v2Match((unsigned char*)mpauInputBuffer + mInputPosition)) {
					unsigned char* buffer = (unsigned char*)mpauInputBuffer + mInputPosition;
					int len =
						((buffer[6] & 0x7f) << 21) |
						((buffer[7] & 0x7f) << 14) |
						((buffer[8] & 0x7f) << 7) |
						(buffer[9] & 0x7f);
					len += ID3v2_HEADER_SIZE;

					while (mInputPosition + len > mInputLength) {
						mInputPosition = 0;
						mInputLength = mFile->read((void *)mpauInputBuffer, MPAUDEC_MAX_AUDIO_FRAME_SIZE);
						if (mInputLength == 0) {
							mEof = true;
							return true;
						}
						len -= mInputLength - mInputPosition;
					}
					mInputPosition += len;
			}

			int r = mpaudec_decode_frame(mpauContext, mpauOutputBuffer, &outputSize, 
				(unsigned char *)mpauInputBuffer + mInputPosition,
				mInputLength - mInputPosition);
			
			
			if (r < 0)
				return false;
			mInputPosition += r;
		}
		/*if (mpauContext->channels != mChannels || mpauContext->sample_rate != mSamplerate) {
		// Can't handle format changes mid-stream.
		printf("Change format");
		return false;
		}*/
		if (!mpauContext->parse_only) {
			if (outputSize < 0) {
				// Couldn't decode this frame.  Too bad, already lost it.
				// This should only happen when seeking.
				outputSize = mpauContext->frame_size;
				memset(mpauOutputBuffer, 0, outputSize * getFrameSize());
			}

												/*short swap;
						int s = outputSize / getFrameSize();
						for (int a = 0; a < --s; a++) {
							swap = ((short *)mpauOutputBuffer)[a];
							((short *)mpauOutputBuffer)[a] = ((short *)mpauOutputBuffer)[s];
							((short *)mpauOutputBuffer)[s] = swap;
						}*/

			/*for(int i=0;i<outputSize/sizeof(short);i++){
			printf("%d ", ((short *)mpauOutputBuffer)[i]);
			}*/
			//LOGE("%d %d", outputSize, stream->position()-startPos);
			mBuffer->write((short *)mpauOutputBuffer, outputSize);
		}
		return true;
	}

	void Mp3Codec::reset(){
		mFile->seek(0);

		mBuffer->clear();

		mEof = false;

		mpaudec_clear(mpauContext);
		mpaudec_init(mpauContext);

		mInputPosition = 0;
		mInputLength = 0;

	}

	bool Mp3Codec::ID3v2Match(unsigned char* buf){
		return  buf[0] == 'I' &&
			buf[1] == 'D' &&
			buf[2] == '3' &&
			buf[3] != 0xff &&
			buf[4] != 0xff &&
			(buf[6] & 0x80) == 0 &&
			(buf[7] & 0x80) == 0 &&
			(buf[8] & 0x80) == 0 &&
			(buf[9] & 0x80) == 0;
	}


