#ifndef __VIDEOSOURCE_H
#define __VIDEOSOURCE_H

#include <iostream>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>

}

#if defined(__APPLE__)
#define AV_NOPTS_VALUE_ ((int64_t)0x8000000000000000LL)
#else
#define AV_NOPTS_VALUE_ ((int64_t)AV_NOPTS_VALUE)
#endif

#include <chrono>
#include <vector>
#include <algorithm>

using namespace std::chrono;
class Frame {
public:
	uint8_t *data;
	Frame(uint8_t *buffer, int width, int height, int channels=4) {
		data = new uint8_t[width*height*channels];
		memcpy(data, buffer, width*height*channels * sizeof(uint8_t));
	}
	~Frame() {
		delete data;
	}
};



class VideoSource {
public:
	AVFormatContext* inctx = nullptr;
	AVCodec* vcodec = nullptr;
	int vstrm_idx;
	AVStream* vstrm;
	int astrm_idx;
	AVStream* astrm;
	SwsContext* swsctx;
	uint8_t *dst_data[4];
	int dst_linesize[4];
	int dst_bufsize;
	AVFrame* frame;
	int64_t picture_pts;

	double eps_zero = 0.000025;

	int dst_width;
	int dst_height;

	AVFrame* decframe;
	int64_t frame_number = 0;
	AVPacket pkt;

	bool reverse = false;

	std::string url;

	std::vector<Frame*> buffer;

	VideoSource() {
		av_register_all();
	}
	VideoSource(std::string infile) {
		load(infile);
	}
	std::string getUrl() {
		return url;
	}

	virtual void load (std::string infile) {
		url = infile;

		picture_pts = AV_NOPTS_VALUE_;


		unload();

		av_log_set_level(AV_LOG_DEBUG);
		int ret;
		inctx = nullptr;
		ret = avformat_open_input(&inctx, infile.c_str(), nullptr, nullptr);
		if (ret < 0) {
			std::cerr << "fail to avforamt_open_input(\"" << infile.c_str() << "\"): ret=" << ret;
			return;
		}
		ret = avformat_find_stream_info(inctx, nullptr);
		if (ret < 0) {
			std::cerr << "fail to avformat_find_stream_info: ret=" << ret;
			return;
		}
		ret = av_find_best_stream(inctx, AVMEDIA_TYPE_VIDEO, -1, -1, &vcodec, 0);
		if (ret < 0) {
			std::cerr << "fail to av_find_best_stream: ret=" << ret;
			return;
		}
		vstrm_idx = ret;
		vstrm = inctx->streams[vstrm_idx];
		ret = avcodec_open2(vstrm->codec, vcodec, nullptr);
		if (ret < 0) {
			std::cerr << "fail to avcodec_open2: ret=" << ret;
			return;
		}

		/*ret = av_find_best_stream(inctx, AVMEDIA_TYPE_AUDIO, -1, -1, &vcodec, 0);
		if (ret < 0) {
			std::cerr << "fail to av_find_best_stream: ret=" << ret;
			return;
		}
		astrm_idx = ret;
		astrm = inctx->streams[astrm_idx];
		ret = avcodec_open2(vstrm->codec, vcodec, nullptr);
		if (ret < 0) {
			std::cerr << "fail to avcodec_open2: ret=" << ret;
			return;
		}*/


		// print input video stream informataion
		std::cout
			<< "infile: " << infile.c_str() << "\n"
			<< "format: " << inctx->iformat->name << "\n"
			<< "vcodec: " << vcodec->name << "\n"
			<< "size:   " << vstrm->codec->width << 'x' << vstrm->codec->height << "\n"
			<< "fps:    " << av_q2d(vstrm->avg_frame_rate) << " [fps]\n"
			<< "length: " << getLengthMs()/1000. << " [sec]\n"
			<< "pixfmt: " << av_get_pix_fmt_name(vstrm->codec->pix_fmt) << "\n"
			<< "frames:  " << getLength() << "\n"
			<< std::flush;
		

		// initialize sample scaler
		//dst_width = vstrm->codec->width;
		//dst_height = vstrm->codec->height;
		dst_width = vstrm->codec->width;
		dst_height = vstrm->codec->height;
		const AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGBA;
		swsctx = sws_getCachedContext(nullptr, vstrm->codec->width, vstrm->codec->height, vstrm->codec->pix_fmt, dst_width, dst_height, dst_pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
		if (!swsctx) {
			std::cerr << "fail to sws_getCachedContext";
			return;
		}
		std::cout << "output: " << dst_width << 'x' << dst_height << ',' << av_get_pix_fmt_name(dst_pix_fmt) << std::endl;


		frame = av_frame_alloc();
		if ((ret = av_image_alloc(dst_data, dst_linesize, dst_width, dst_height, dst_pix_fmt, 1)) < 0) {
			std::cerr << "Could not allocate destination image";
		}
		dst_bufsize = ret;


		//av_image_fill_arrays(frame->data, frame->linesize, framebuf.data(), dst_pix_fmt, dst_width, dst_height, 1);
		decframe = av_frame_alloc();

		loopEnd = getLength();
	
		/*for (auto f : buffer) {
			delete f;
		}
		buffer.clear();
		while(grabFrame()) {
			auto ptr = scale();
			buffer.push_back(new Frame(ptr, getWidth(), getHeight(), getChannels()));
		}*/
		frame_number = 0;
	}
	virtual void unload() {

		if (vstrm) {
			avcodec_close(vstrm->codec);
		}

		if (inctx) {
			avformat_free_context(inctx);
			inctx = nullptr;
			vstrm = nullptr;
		}


		if(frame)
			av_frame_free(&frame);
		if (decframe)
			av_frame_free(&decframe);

		av_free_packet(&pkt);

		if (swsctx) {
			sws_freeContext(swsctx);
			swsctx = nullptr;
		}


	}

	virtual ~VideoSource(){}
	int64_t getLength(){
		if (!vstrm) return 0;
		int64_t nbf = vstrm->nb_frames;
		if (nbf == 0)
		{
			nbf = (int64_t)floor((getLengthMs()/1000) * getFramerate() + 0.5);
		}
		return nbf;
	}
	double getLengthMs() {
		if (!vstrm) return 0;
		double sec = (double)inctx->duration / (double)AV_TIME_BASE;
		if (sec < eps_zero){
			sec = (double)vstrm->duration * av_q2d(vstrm->time_base);
		}
		if (sec < eps_zero){
			sec = (double)vstrm->duration * av_q2d(vstrm->time_base);
		}
		return sec*1000.;
	}


	double getFramerate() {
		if (!vstrm) return 0;
			double fps = av_q2d(vstrm->avg_frame_rate);

			if (fps < eps_zero)
			{
				fps = 1.0 / av_q2d(vstrm->codec->time_base);
			}

			return fps;
	}
	unsigned short getChannels() { return 4; }
	unsigned short getBits() { return 24; }

	unsigned short getFrameSize();
	unsigned short getPixelSize();



	virtual uint8_t* read() {
		if (!vstrm) return nullptr;
			if (frame_number <= loopStart) {
				reverse = false;
				seek(loopStart);
			}
			if (frame_number >= loopEnd && loopEnd >= loopStart) {
				seek(loopStart);

				//reverse = true;
			}
			if (reverse) {
				if (frame_number >= loopEnd && loopEnd >= loopStart) {
					seek(loopEnd-2);
				}else {
					seek(frame_number - 1);
				}
                if (buffer.size() > 0) return buffer[frame_number++]->data;
				return scale();
			}

		if (buffer.size() > 0) return buffer[frame_number++]->data;
		if (grabFrame()) {
			return scale();
		}

		return nullptr;
	};
	virtual uint8_t* scale() {
		sws_scale(swsctx, decframe->data, decframe->linesize, 0, decframe->height, dst_data, dst_linesize);
		return dst_data[0];
	}

	bool grabFrame()
	{
		bool valid = false;
		int got_picture;

		int count_errs = 0;
		const int max_number_of_attempts = 1 << 9;

		//if (!ic || !video_st)  return false;

		if (vstrm->nb_frames > 0 &&
			frame_number > vstrm->nb_frames)
			return false;

		picture_pts = AV_NOPTS_VALUE_;


		// get the next frame
		while (!valid)
		{

			av_packet_unref(&pkt);


			int ret = av_read_frame(inctx, &pkt);
			if (ret == AVERROR(EAGAIN)) continue;

			/* else if (ret < 0) break; */

			if (pkt.stream_index != vstrm_idx)
			{
				av_packet_unref(&pkt);
				count_errs++;
				if (count_errs > max_number_of_attempts)
					break;
				continue;
			}


			avcodec_decode_video2(vstrm->codec, decframe, &got_picture, &pkt);


			// Did we get a video frame?
			if (got_picture)
			{
				//picture_pts = picture->best_effort_timestamp;
				if (picture_pts == AV_NOPTS_VALUE_)
					picture_pts = decframe->pkt_pts != AV_NOPTS_VALUE_ && decframe->pkt_pts != 0 ? decframe->pkt_pts : decframe->pkt_dts;

				//std::cout << frame_number << '\r' << std::flush;  // dump progress
				frame_number++;
				valid = true;
			}
			else
			{
				count_errs++;
				if (count_errs > max_number_of_attempts)
					break;
			}
		}

		if (valid && first_frame_number < 0)
			first_frame_number = dts_to_frame_number(picture_pts);

		// return if we have a new picture or not
		return valid;
	}



	int64_t first_frame_number = -1;
	virtual void seek(int64_t _frame_number, bool acurate = true)
	{
        if (buffer.size() > 0) {
            frame_number = Utils::clamp(_frame_number, 0, getLength());
            return;
        }
		if (!vstrm) return;


		_frame_number = std::min(_frame_number, getLength());
		int delta = 16;

		// if we have not grabbed a single frame before first seek, let's read the first frame
		// and get some valuable information during the process
		if (first_frame_number < 0 && getLength() > 1)
			grabFrame();

		for (;;)
		{
			int64_t _frame_number_temp = std::max(_frame_number - delta, (int64_t)0);
			double sec = (double)_frame_number_temp / getFramerate();
			int64_t time_stamp = vstrm->start_time;
			double  time_base = av_q2d(vstrm->time_base);
			time_stamp += (int64_t)(sec / time_base + 0.5);
			//if (getLength() > 1) av_seek_frame(inctx, vstrm_idx, _frame_number -2, AVSEEK_FLAG_FRAME);
			if (getLength() > 1) av_seek_frame(inctx, vstrm_idx, time_stamp, AVSEEK_FLAG_BACKWARD);
			avcodec_flush_buffers(vstrm->codec);
			if (_frame_number > 0)
			{
				grabFrame();

				if (_frame_number > 1)
				{
					frame_number = dts_to_frame_number(picture_pts) - first_frame_number;
					//printf("_frame_number = %d, frame_number = %d, delta = %d\n",
					       //(int)_frame_number, (int)frame_number, delta);
						   //printf("diff = %d\n", _frame_number - frame_number);

					if (!acurate) break;

					if (frame_number < 0 || frame_number > _frame_number - 1)
					{
						if (_frame_number_temp == 0 || delta >= INT_MAX / 4)
							break;
						delta = delta < 16 ? delta * 2 : delta * 3 / 2;
						continue;
					}
					while (frame_number < _frame_number - 1)
					{
						if (!grabFrame())
							break;
					}
					frame_number++;
					break;
				}
				else
				{
					frame_number = 1;
					break;
				}
			}
			else
			{
				frame_number = 0;
				break;
			}
		}
	};
	int64_t dts_to_frame_number(int64_t dts)
	{
		double sec = dts_to_sec(dts);
		return (int64_t)(getFramerate() * sec + 0.5);
	}
	double dts_to_sec(int64_t dts)
	{
		return (double)(dts - vstrm->start_time) *
			av_q2d(vstrm->time_base);
	}

	virtual int getPosition() { return frame_number; }
	virtual void setPosition(int position) {}

	void setLength(unsigned long lenght);
	void setSamplerate(double samplerate);
	void setChannels(unsigned short channels);
	void setBits(unsigned short bits);

	double mPosition;

	unsigned long mLenght;
	double mFramerate;
	unsigned short mChannels;
	unsigned short mBits;


	int getWidth() {
		return dst_width;
	}
	int getHeight() {
		return dst_height;
	}
public:
	long loopStart = 1;
	long loopEnd = -1;

	virtual void setLoopStart(long start) {
		loopStart = Utils::clamp(start, 0, loopEnd-4);
	}
	virtual void setLoopEnd(long end) {
		loopEnd = Utils::clamp(end, loopStart+4, getLength());
	}

	double getAspectRatio() {
		return (double)dst_width / (double)dst_height;
	}
};

#endif
