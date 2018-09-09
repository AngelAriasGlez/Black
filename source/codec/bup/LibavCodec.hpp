#ifndef _FFMPPEGCODEC_H
#define _FFMPPEGCODEC_H

#include "Codec.hpp"
/*
extern "C" {
        #include <libavformat/avformat.h>
        #include <libavcodec/avcodec.h>
        #include <libavutil/opt.h>
        #include <libavutil/mathematics.h>
        #include <libavresample/avresample.h>
    }

#include "utils/BidirectionalBuffer.h"

class LibavCodec : public Codec {
private:


	int64_t m_iSeekOffset = -1;
	int64_t m_iLastFirstFfmpegByteOffset = -1;
	int64_t m_iCurrentTs = -1;
	int64_t m_iOffset = -1;
	int m_bIsSeeked = 0;

	int64_t mCurrentFrame;
	int64_t mSeekFrame;

	AVFormatContext *m_pFormatCtx;
	AVInputFormat *m_pIformat;
	AVCodecContext *m_pCodecCtx;
	int m_iAudioStream;
	AVCodec *m_pCodec;
	int64_t m_lPcmLength;


	enum AVSampleFormat m_pOutSampleFmt;
	enum AVSampleFormat m_pInSampleFmt;




	AVAudioResampleContext *m_pSwrCtx;
	uint8_t *m_pOut;
	unsigned int m_pOutSize;

	BidirectionalBuffer<Sample> mBuffer;


public:


	LibavCodec();
	~LibavCodec();

	bool _open();
	int _read(void* pointer, int frameCount);
	void _seek(int frameNumber);


	double fe_convert_toByteOffset(double pts, unsigned int samplerate, const AVRational *ffmpegtime) {
		return (pts / (double)ffmpegtime->den * (double)samplerate *
			(double)2.);
	}

	double fe_convert_fromByteOffset(double byteoffset, unsigned int samplerate, const AVRational *ffmpegtime) {
		return (byteoffset / (double)samplerate / (double)2.) *
			(double)ffmpegtime->den;
	}

	int fe_resample_open(enum AVSampleFormat inSampleFmt,
	enum AVSampleFormat outSampleFmt) {
		m_pOutSampleFmt = outSampleFmt;
		m_pInSampleFmt = inSampleFmt;


		// Some MP3/WAV don't tell this so make assumtion that
		// They are stereo not 5.1
		if (m_pCodecCtx->channel_layout == 0 && m_pCodecCtx->channels == 2) {
			m_pCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;

		}
		else if (m_pCodecCtx->channel_layout == 0 && m_pCodecCtx->channels == 1) {
			m_pCodecCtx->channel_layout = AV_CH_LAYOUT_MONO;

		}
		else if (m_pCodecCtx->channel_layout == 0 && m_pCodecCtx->channels == 0) {
			m_pCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
			m_pCodecCtx->channels = 2;
		}

		LOGI("fe_resample_open: Rate: %d Channels %d Channels: %d\n",
			(int)m_pCodecCtx->sample_rate,
			(int)m_pCodecCtx->channel_layout,
			(int)m_pCodecCtx->channels);

		// They make big change in FFPEG 1.1 before that every format just passed
		// s16 back to application from FFMPEG 1.1 up MP3 pass s16p (Planar stereo
		// 16 bit) MP4/AAC FLTP (Planar 32 bit float) and OGG also FLTP
		// (WMA same thing) If sample type aint' S16 (Stero 16) example FFMPEG 1.1
		// mp3 is s16p that ain't and mp4 FLTP (32 bit float)
		// NOT Going to work because MIXXX works with pure s16 that is not planar
		// GOOD thing is now this can handle allmost everything..
		// What should be tested is 44800 Hz downsample and 22100 Hz up sample.
		if ((inSampleFmt != outSampleFmt || m_pCodecCtx->sample_rate != 44100 ||
			m_pCodecCtx->channel_layout != AV_CH_LAYOUT_STEREO) && m_pSwrCtx == NULL) {
			if (m_pSwrCtx != NULL) {
				LOGI("fe_resample_open: Freeing Resample context\n");

				// __FFMPEGOLDAPI__ Is what is used in FFMPEG < 0.10 and libav < 0.8.3. NO
				// libresample available..



				avresample_close(m_pSwrCtx);

				m_pSwrCtx = NULL;
			}

			// Create converter from in type to s16 sample rate

			LOGI("fe_resample_open: ffmpeg: NEW FFMPEG API using libavresample\n");
			m_pSwrCtx = avresample_alloc_context();


			av_opt_set_int(m_pSwrCtx, "in_channel_layout", m_pCodecCtx->channel_layout, 0);
			av_opt_set_int(m_pSwrCtx, "in_sample_fmt", inSampleFmt, 0);
			av_opt_set_int(m_pSwrCtx, "in_sample_rate", m_pCodecCtx->sample_rate, 0);
			av_opt_set_int(m_pSwrCtx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
			av_opt_set_int(m_pSwrCtx, "out_sample_fmt", outSampleFmt, 0);
			av_opt_set_int(m_pSwrCtx, "out_sample_rate", m_pCodecCtx->sample_rate, 0);



			if (!m_pSwrCtx) {
				LOGI("fe_resample_open: Can't init convertor!\n");
				return -1;
			}


			if (avresample_open(m_pSwrCtx) < 0) {

				LOGI("fe_resample_open: Can't init convertor\n");
				m_pSwrCtx = NULL;
				return -1;
			}


		}

		LOGI("fe_resample_open: From Sample: %d Hz Sample format: %s",
			m_pFormatCtx->streams[m_iAudioStream]->codec->sample_rate,
			av_get_sample_fmt_name(inSampleFmt));

		LOGI(" to 44100 Sample format: %s\n",
			av_get_sample_fmt_name(outSampleFmt));

		return 0;
	}

	unsigned int fe_resample_do(AVFrame *inframe) {

		if (m_pSwrCtx) {


#if LIBAVRESAMPLE_VERSION_MAJOR == 0
			void **l_pIn = (void **)inframe->extended_data;
#else
			uint8_t **l_pIn = (uint8_t **)inframe->extended_data;
#endif


			// Left here for reason!
			// Sometime in time we will need this!
			//#else
			//        int64_t l_lInReadBytes = av_samples_get_buffer_size(NULL, m_pCodecCtx->channels,
			//                                 inframe->nb_samples,
			//                                 m_pCodecCtx->sample_fmt, 1);


			int l_iOutBytes = 0;

			int l_iOutSamples = av_rescale_rnd(avresample_get_delay(m_pSwrCtx) +
				inframe->nb_samples,
				m_pCodecCtx->sample_rate,
				m_pCodecCtx->sample_rate,
				AV_ROUND_UP);

			int l_iOutSamplesLines = 0;

			// Alloc too much.. if not enough we are in trouble!
			if (av_samples_alloc(&m_pOut, &l_iOutSamplesLines, 2, l_iOutSamples,
				m_pOutSampleFmt, 1) < 0) {
				LOGI("fe_resample_do: Alloc not succeedeed\n");
				return -1;
			}


			int l_iLen = 0;



			// OLD API (0.0.3) ... still NEW API (1.0.0 and above).. very frustrating..
			// USED IN FFMPEG 1.0 (LibAV SOMETHING!). New in FFMPEG 1.1 and libav 9
#if LIBAVRESAMPLE_VERSION_INT <= 3
			// AVResample OLD
			l_iLen = avresample_convert(m_pSwrCtx, (void **)&m_pOut, 0, l_iOutSamples,
				(void **)l_pIn, 0, inframe->nb_samples);
#else
			//AVResample NEW
			l_iLen = avresample_convert(m_pSwrCtx, (uint8_t **)&m_pOut, 0, l_iOutSamples,
				(uint8_t **)l_pIn, 0, inframe->nb_samples);

#endif


			l_iOutBytes = av_samples_get_buffer_size(NULL, 2, l_iLen, m_pOutSampleFmt, 1);



			if (l_iLen < 0) {
				LOGI("Sample format conversion failed!\n");
				return -1;
			}

			m_pOutSize = l_iOutBytes;
			return l_iOutBytes;

		}
		else {
			return 0;
		}

		return 0;
	}

};
*/
#endif
