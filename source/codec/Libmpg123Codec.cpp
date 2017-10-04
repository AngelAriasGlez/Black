


#include "Libmpg123Codec.hpp"
#define DEBUG
#include "../utils/Utils.hpp"

Libmpg123Codec::Libmpg123Codec() {
	mpg123_init();
	mHandle = mpg123_new(NULL, &mResult);
	if(mHandle == NULL){
	  LOGE("Libmpg123Codec: Unable to create mpg123 handle: %s\n", mpg123_plain_strerror(mResult));
	}
	//mpg123_param(mHandle, MPG123_RESYNC_LIMIT, -1, 0); /* New in library version 0.0.1 . */
	mpg123_param(mHandle, MPG123_VERBOSE, 2, 0);
	mpg123_param(mHandle, MPG123_FLAGS, MPG123_SKIP_ID3V2, 0);

}
Libmpg123Codec::~Libmpg123Codec() {
	mpg123_delete(mHandle);
	mpg123_exit();
}

bool Libmpg123Codec::_open() {
	mResult = mpg123_open(mHandle, getUri().c_str());
	if(MPG123_OK != mResult){
		LOGE("Libmpg123Codec: %s, %s", mpg123_strerror(mHandle), getUri().c_str());
		return false;
	}
	LOGE("-1-----");
	mpg123_scan(mHandle);
	LOGE("-2-----");
	long samplerate;
	int channels, enc;
	mpg123_getformat(mHandle, &samplerate, &channels, &enc);
	LOGE("-3-----");
	setSamplerate(samplerate);
	setChannels(channels);
	setLength(mpg123_length(mHandle));
	LOGE("-4-----");
	return true;
}


void Libmpg123Codec::_seek(int frameNumber) {
	mpg123_seek(mHandle, frameNumber, SEEK_SET);
}
int Libmpg123Codec::_read(void* pointer, int frameCount) {
	int toRead = abs(frameCount);
	if (frameCount < 0) {
		_seek(getPosition() - toRead);
	}
	size_t readed;
	mpg123_read(mHandle, (unsigned char*)pointer, frameCount*getFrameSize(), &readed);
	
	return readed/getFrameSize();
	


}



