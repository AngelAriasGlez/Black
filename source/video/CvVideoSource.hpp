#ifndef __CVVIDEOSOURCE_H
#define __CVVIDEOSOURCE_H

#ifdef __OPENCV

#include <opencv2/opencv.hpp> 
#include "utils/Log.hpp"
#include <chrono>
#include <vector>

#include "VideoSource.hpp"

class CvVideoSource : public VideoSource{
public:
	cv::VideoCapture mCvCap;
	std::string url;

	int dst_width;
	int  dst_height;

	cv::Mat rgbFrame;

	CvVideoSource() {

	}
	CvVideoSource(std::string infile) {
		load(infile);
	}
	std::string getUrl() {
		return url;
	}

    virtual void load (std::string infile)   override {
		url = infile;

		mCvCap.open(infile);
		mCvCap.set(CV_CAP_PROP_CONVERT_RGB, true);
		dst_width = mCvCap.get(CV_CAP_PROP_FRAME_WIDTH);
		dst_height = mCvCap.get(CV_CAP_PROP_FRAME_HEIGHT);

		int f = mCvCap.get(CV_CAP_PROP_FORMAT);
		rgbFrame = cv::Mat(dst_height, dst_width, CV_8UC4);


		/*std::vector<int> param(2);
		param[0] = cv::IMWRITE_JPEG_QUALITY;
		param[1] = 60;//default(95) 0-100

		double total = mCvCap.get(CV_CAP_PROP_FRAME_COUNT);
		while (true) {
			mCvCap >> rgbFrame;
			if (rgbFrame.empty()) break;
			auto buffer = new std::vector<uchar>();
			cv::imencode(".jpg", rgbFrame, *buffer, param);
			frames.push_back(buffer);
			LOGE("%f%", 100. / total*(double)frames.size());
		}*/

	}
	virtual void unload() {
		mCvCap.release();
		rgbFrame.release();
	}

	virtual ~CvVideoSource(){
		unload();
	}

	uint64_t getLength() override{
		//return frames.size();
		return mCvCap.get(CV_CAP_PROP_FRAME_COUNT);
	}

	double getFramerate() override{
		return mCvCap.get(CV_CAP_PROP_FPS);
	}

	//uint64_t position;
	//std::vector<std::vector<uchar>*> frames;
	//cv::Mat imgmat;
	virtual uint8_t* read() override {
		//imgmat = cv::imdecode((cv::InputArray)*frames[position++], CV_LOAD_IMAGE_UNCHANGED);
		//return imgmat.data;

		mCvCap >> rgbFrame;
		return rgbFrame.data;

	};



	unsigned short getChannels()   override{ return 3; }

	virtual void seek(uint64_t  _frame_number, bool acurate = true)   override
	{
		//position = _frame_number;
		
		mCvCap.set(CV_CAP_PROP_POS_FRAMES, _frame_number);

	};

	virtual uint64_t getPosition() override {
		//return position;
		return mCvCap.get(CV_CAP_PROP_POS_FRAMES);
	}


	uint64_t getWidth()  override {
		return dst_width;
	}
	uint64_t getHeight()  override {
		return dst_height;
	}
};

#endif
#endif
