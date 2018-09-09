#include "LibavCodec.hpp"
#include <iostream>
#include "../utils/Utils.hpp"

/*

LibavCodec::LibavCodec() {
	m_pFormatCtx = NULL;
	m_pIformat = NULL;
	m_pCodecCtx = NULL;
	m_iAudioStream = -1;
	m_pCodec = NULL;
	m_lPcmLength = 0;


	m_pSwrCtx = NULL;
	m_pOut = NULL;
	m_pOutSize = -1;

	m_iSeekOffset = -1;
	m_iLastFirstFfmpegByteOffset = -1;
	m_iCurrentTs = -1;
	m_iOffset = -1;
	m_bIsSeeked = 0;

	mCurrentFrame = 0;
	mSeekFrame = -1;

	// Initialize libavcodec, and register all codecs and formats.
	av_register_all();
}
LibavCodec::~LibavCodec() {
	// Clean up!
	avcodec_close(m_pCodecCtx);
	avformat_close_input(&m_pFormatCtx);



}

bool LibavCodec::_open() {
	int i = -1;
	AVDictionary *l_iFormatOpts = NULL;

	LOGI("fe_decode_open: Decode audio file %s\n",
		getUri().c_str());

	m_pFormatCtx = avformat_alloc_context();

	// Enable this to use old slow MP3 Xing TOC
#ifndef CODEC_ID_MP3

	if (LIBAVFORMAT_VERSION_INT > 3540580) {
		LOGI("fe_decode_open: Set usetoc to have old way of XING TOC reading (libavformat version: '%d')\n", LIBAVFORMAT_VERSION_INT);
		av_dict_set(&l_iFormatOpts, "usetoc", "0", 0);
	}

#endif

	// Open file and make m_pFormatCtx
	if (avformat_open_input(&m_pFormatCtx, getUri().c_str(), NULL, &l_iFormatOpts) != 0) {
		LOGI("fe_decode_open: cannot open: %s\n",
			getUri().c_str());
		return false;
	}

#ifndef CODEC_ID_MP3

	if (LIBAVFORMAT_VERSION_INT > 3540580 && l_iFormatOpts != NULL) {
		av_dict_free(&l_iFormatOpts);
	}

#endif

#if LIBAVCODEC_VERSION_INT < 3622144
	m_pFormatCtx->max_analyze_duration = 999999999;
#endif

	// Retrieve stream information
	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0) {
		LOGI("fe_decode_open: cannot open %s\n",
			getUri().c_str());
		return false;
	}

	av_dump_format(m_pFormatCtx, 0, getUri().c_str(), 0);

	// Find the first video stream
	m_iAudioStream = -1;

	for (i = 0; i < m_pFormatCtx->nb_streams; i++) {
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			m_iAudioStream = i;
			break;
		}
	}

	if (m_iAudioStream == -1) {
		LOGI("fe_decode_open: cannot find an audio stream: cannot open %s",
			getUri().c_str());
		return false;
	}

	// Get a pointer to the codec context for the video stream
	m_pCodecCtx = m_pFormatCtx->streams[m_iAudioStream]->codec;

	// Find the decoder for the audio stream
	if (!(m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id))) {
		LOGI("fe_decode_open: cannot find a decoder for %s\n",
			getUri().c_str());
		return false;
	}


	if (avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0) {
		LOGI("fe_decode_open:  cannot open %s\n",
			getUri().c_str());
		return false;
	}



	fe_resample_open(m_pCodecCtx->sample_fmt, AV_SAMPLE_FMT_FLT);


	setSamplerate(m_pCodecCtx->sample_rate);
	setChannels(m_pCodecCtx->channels);

	m_lPcmLength = (long int)((double)m_pFormatCtx->duration * 2 / AV_TIME_BASE *
		getSamplerate());


	setLength(m_lPcmLength);

	return true;

}
int LibavCodec::_read(void* pointer, int frameCount) {

	if (mBuffer.getSize() < frameCount * getChannels() * 10) {
		mBuffer.resize(frameCount * getChannels() * 10);
	}

	AVPacket packet;
	AVFrame *frame = av_frame_alloc();

	int samplesToRead = frameCount * getChannels();

	packet.data = NULL;
	packet.size = 0;

	while (mBuffer.avaliableForward() < samplesToRead) {
		av_init_packet(&packet);

		if (av_read_frame(m_pFormatCtx, &packet) >= 0) {
			if (packet.stream_index == m_iAudioStream) {


				int frameFinished = 0;
				int ret = avcodec_decode_audio4(m_pCodecCtx, frame, &frameFinished, &packet);

				if (ret <= 0) {
					break;
				}

				//frame->
				if (frameFinished) {
					fe_resample_do(frame);
					mCurrentFrame = frame->pkt_dts;

					mBuffer.pushRight((Sample *)m_pOut, frame->linesize[0]);
				}else{
					//LOGI("fe_read_frame: libavcodec 'avcodec_decode_audio4' didn't succeed or frame not finished (File could also just end!)\n");
				}

			}

		}
		else {
			//LOGI("fe_read_frame: libavcodec 'av_read_frame' didn't succeed!\n");
			//break;
		}

		av_free_packet(&packet);
		packet.data = NULL;
		packet.size = 0;

	}

	av_free(packet.data);
	packet.data = NULL;
	packet.size = 0;

	av_frame_unref(frame);

	return  mBuffer.readForward((Sample *)pointer, samplesToRead) / getChannels();

}
void LibavCodec::_seek(int frameNumber) {
	int tsms = (frameNumber / 2) / getSamplerate() * 1000;
	int64_t dts = av_rescale(tsms, m_pFormatCtx->streams[m_iAudioStream]->time_base.den, m_pFormatCtx->streams[m_iAudioStream]->time_base.num) / 1000;

	int flags = AVSEEK_FLAG_FRAME;
	if (dts < mCurrentFrame)
	{
		flags |= AVSEEK_FLAG_BACKWARD;
	}

	if (av_seek_frame(m_pFormatCtx, m_iAudioStream, dts, flags)){

		return;
	}
	avcodec_flush_buffers(m_pCodecCtx);
	mSeekFrame = dts;


	/*mBuffer.clear();


	int filepos = frameNumber;
	int ret = 0;
	int64_t fspos = 0;
	int64_t l_lSeekPos = 0;
	int64_t minus = filepos;
	AVRational time_base = m_pFormatCtx->streams[m_iAudioStream]->time_base;

	fspos = fe_convert_fromByteOffset((double)minus, 44100, &time_base);

	m_iCurrentTs = filepos;

	m_iOffset = 0;

	avcodec_flush_buffers(m_pCodecCtx);

	ret = avformat_seek_file(m_pFormatCtx,
		m_iAudioStream,
		0,
		fspos,
		fspos,
		AVSEEK_FLAG_BACKWARD);


	avcodec_flush_buffers(m_pCodecCtx);

	if (ret < 0) {
		LOGI("fe_read_seek: Seek ERROR ret(%d) filepos(%ld)\n",
			ret,
			filepos);
		return;
	}

	l_lSeekPos = (int64_t)round(fe_convert_toByteOffset(
		m_pFormatCtx->streams[m_iAudioStream]->cur_dts, 44100, &time_base));

	m_iLastFirstFfmpegByteOffset = l_lSeekPos;


	m_iSeekOffset = m_iOffset = (int64_t)round(fe_convert_toByteOffset((double)(fspos - m_pFormatCtx->streams[m_iAudioStream]->cur_dts),
		44100, &time_base));

	m_iSeekOffset = (int64_t)round((double)l_lSeekPos / 2304);
	m_iSeekOffset *= 2304;

	LOGI("fe_read_seek: (RBP: %ld/%.4f) Req Byte: %ld (Sec: %.4f PTS %ld) Real PTS %ld (Sec: %.4f Byte: %ld)\n",
		filepos,
		(double)((double)filepos / (double)88200),
		minus,
		(double)((double)minus / (double)88200),
		fspos,
		m_pFormatCtx->streams[m_iAudioStream]->cur_dts, m_pFormatCtx->streams[m_iAudioStream]->cur_dts * av_q2d(m_pFormatCtx->streams[m_iAudioStream]->time_base),
		(int64_t)round(fe_convert_toByteOffset((double)m_pFormatCtx->streams[m_iAudioStream]->cur_dts, 44100, &time_base)));

	m_bIsSeeked = 1;*/

//}
