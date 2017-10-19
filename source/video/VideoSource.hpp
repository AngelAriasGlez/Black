#ifndef __VIDEOSOURCE_H
#define __VIDEOSOURCE_H

#include <string>
class VideoSource {
public:
	VideoSource() {}

	virtual uint64_t getLength() = 0;
	virtual uint64_t getPosition() = 0;
	virtual uint64_t getWidth() = 0;
	virtual uint64_t getHeight() = 0;
	virtual std::string getUrl() = 0;
	virtual void load(std::string infile) = 0;
	virtual void seek(uint64_t  pos, bool precise = true) = 0;
	virtual uint8_t* read() = 0;
	virtual double getFramerate() = 0;
	virtual unsigned short getChannels() = 0;
	virtual double getAspectRatio() {
		//Reverse if portrait or landscape
		return isPortrait() ? (double)getHeight() / (double)getWidth() :(double)getWidth() / (double)getHeight();
	};
	virtual double isPortrait() {
		return getRotation() == 90 || getRotation() == 270;
	}
	virtual double getRotation() = 0;

	virtual uint64_t getFramePixelCount() {
		return getWidth()*getHeight()*getChannels();
	}

};
#endif
