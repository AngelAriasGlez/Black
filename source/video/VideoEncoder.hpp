#ifndef __VIDEOENCODER_H
#define __VIDEOENCODER_H

#include <stdio.h>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/common.h>
#include  <libavcodec/avcodec.h>
#include <libavutil/imgutils.h> //for av_image_alloc only
#include <libavutil/opt.h>


}
enum AVRounding2 {
	AV_ROUND_ZERO = 0,
	AV_ROUND_INF = 1,
	AV_ROUND_DOWN = 2,
	AV_ROUND_UP = 3,
	AV_ROUND_NEAR_INF = 5,
};


#define OUTPUT_PIX_FMT AV_PIX_FMT_YUV420P

class VideoEncoder {
private:
	AVFormatContext *fmt_ctx;
	AVCodecContext  *codec_ctx; //a shortcut to st->codec
	AVStream        *st;
	AVFrame         *tmp_frame;
	int              pts = 0;
public:
	VideoEncoder(int width, int height, const char* target, double fps) {

		int err;
		AVOutputFormat  *fmt;
		AVCodec         *codec;
		AVDictionary *fmt_opts = NULL;
		av_register_all();
		this->fmt_ctx = avformat_alloc_context();
		if (this->fmt_ctx == NULL) {
			std::cerr << "Error : can not alloc av context" << std::endl;
		}
		//init encoding format
		fmt = av_guess_format(NULL, target, NULL);
		if (!fmt) {
			std::cout << "Could not deduce output format from file extension: using MPEG4." << std::endl;
			fmt = av_guess_format("mpeg4", NULL, NULL);
		}
		//std::cout <<fmt->long_name<<std::endl;
		//Set format header infos
		fmt_ctx->oformat = fmt;
		//snprintf(fmt_ctx->filename, sizeof(fmt_ctx->filename), "%s", target);
		//Reference for AvFormatContext options : https://ffmpeg.org/doxygen/2.8/movenc_8c_source.html
		//Set format's privater options, to be passed to avformat_write_header()
		err = av_dict_set(&fmt_opts, "movflags", "faststart", 0);
		if (err < 0) {
			std::cerr << "Error : " << "av_dict_set movflags" << std::endl;
		}
		//default brand is "isom", which fails on some devices
		av_dict_set(&fmt_opts, "brand", "mp42", 0);
		if (err < 0) {
			std::cerr << "Error : " << "av_dict_set brand" << std::endl;
		}
		codec = avcodec_find_encoder(fmt->video_codec);

		if (!codec) {
			throw "can't find encoder";
		}
		if (!(st = avformat_new_stream(fmt_ctx, codec))) {
			throw "can't create new stream";
		}
		//set stream time_base
		/* frames per second FIXME use input fps? */
		AVRational ra{100,(int)(fps*100)};

		st->time_base = ra;

		//Set codec_ctx to stream's codec structure
		codec_ctx = st->codec;
		/* put sample parameters */
		codec_ctx->sample_fmt = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_S16;
		codec_ctx->width = width;
		codec_ctx->height = height;
		codec_ctx->time_base = st->time_base;
		codec_ctx->pix_fmt = OUTPUT_PIX_FMT;
		codec_ctx->flags |= (1 << 22); //AVFMT_GLOBALHEADER
		/* Apparently it's in the example in master but does not work in V11
		if (o_format_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		*/
		//H.264 specific options
		codec_ctx->gop_size = 25;
		codec_ctx->level = 31;
		err = av_opt_set(codec_ctx->priv_data, "crf", "12", 0);
		if (err < 0) {
			std::cerr << "Error : " << "av_opt_set crf" << std::endl;
		}
		err = av_opt_set(codec_ctx->priv_data, "profile", "main", 0);
		if (err < 0) {
			std::cerr << "Error : " << "av_opt_set profile" << std::endl;
		}
		err = av_opt_set(codec_ctx->priv_data, "preset", "slow", 0);
		if (err < 0) {
			std::cerr << "Error : " << "av_opt_set preset" << std::endl;
		}
		// disable b-pyramid. CLI options for this is "-b-pyramid 0"
		//Because Quicktime (ie. iOS) doesn't support this option
		err = av_opt_set(codec_ctx->priv_data, "b-pyramid", "0", 0);
		if (err < 0) {
			std::cerr << "Error : " << "av_opt_set b-pyramid" << std::endl;
		}
		//It's necessary to open stream codec to link it to "codec" (the encoder).
		err = avcodec_open2(codec_ctx, codec, NULL);
		if (err < 0) {
			std::cerr << "Error : " << "avcodec_open2" << std::endl;
		}

		//* dump av format informations
		av_dump_format(fmt_ctx, 0, target, 1);
		//*/
		err = avio_open(&fmt_ctx->pb, target, AVIO_FLAG_WRITE);
		if (err < 0) {
			std::cerr << "Error : " << "avio_open" << std::endl;
		}

		//Write file header if necessary
		err = avformat_write_header(fmt_ctx, &fmt_opts);
		if (err < 0) {
			std::cerr << "Error : " << "avformat_write_header" << std::endl;
		}


	}

	void close() {
		int err;
		std::cout << "cleaning Encoder" << std::endl;
		//Write pending packets
		while ((err = write((AVFrame*)NULL)) == 1) {};
		if (err < 0) {
			std::cout << "error writing delayed frame" << std::endl;
		}
		//Write file trailer before exit
		av_write_trailer(this->fmt_ctx);
		//close file

	}

	~VideoEncoder() {

		avio_close(fmt_ctx->pb);
		
		avcodec_close(codec_ctx);
		avformat_free_context(this->fmt_ctx);
		//avcodec_free_context(&this->codec_ctx);


	}

	int write(AVFrame *frame) {
		int err;
		int got_output = 1;
		AVPacket pkt = { 0 };
		av_init_packet(&pkt);

		//Set frame pts, monotonically increasing, starting from 0
		if (frame != NULL) frame->pts = pts++; //we use frame == NULL to write delayed packets in destructor
		err = avcodec_encode_video2(this->codec_ctx, &pkt, frame, &got_output);
		if (err < 0) {
			std::cout << "encode frame" << std::endl;
			return err;
		}
		if (got_output) {

			av_packet_rescale_ts(&pkt, this->codec_ctx->time_base, this->st->time_base);
			pkt.stream_index = this->st->index;
			/* write the frame */
			//printf("Write packet %03d of size : %05d\n",pkt.pts,pkt.size);
			//write_frame will take care of freeing the packet.
			err = av_interleaved_write_frame(this->fmt_ctx, &pkt);
			if (err < 0) {
				std::cout << "write frame" << std::endl;
				return err;
			}
			return 1;
		}
		else {
			return 0;
		}
	}
	void av_packet_rescale_ts(AVPacket *pkt, AVRational src_tb, AVRational dst_tb){
			if (pkt->pts != AV_NOPTS_VALUE)
				pkt->pts = av_rescale_q(pkt->pts, src_tb, dst_tb);
			if (pkt->dts != AV_NOPTS_VALUE)
				 pkt->dts = av_rescale_q(pkt->dts, src_tb, dst_tb);
			if (pkt->duration > 0)
				pkt->duration = av_rescale_q(pkt->duration, src_tb, dst_tb);
			if (pkt->convergence_duration > 0)
				pkt->convergence_duration = av_rescale_q(pkt->convergence_duration, src_tb, dst_tb);
	}
	int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq) {
		int64_t b = bq.num * (int64_t)cq.den;
		int64_t c = cq.num * (int64_t)bq.den;
		return av_rescale_rnd(a, b, c, AV_ROUND_NEAR_INF);
	}

	int64_t av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding2 rnd) {
			int64_t r = 0;

				if (a<0 && a != INT64_MIN) return -av_rescale_rnd(-a, b, c, (AVRounding2)(rnd ^ ((rnd >> 1) & 1)));
					if (rnd == AV_ROUND_NEAR_INF) r = c / 2;
				else if (rnd & 1)             r = c - 1;
		
				if (b <= INT_MAX && c <= INT_MAX) {
					if (a <= INT_MAX)
						return (a * b + r) / c;
					else
						return a / c*b + (a%c*b + r) / c;
		}else {
					uint64_t a0 = a & 0xFFFFFFFF;
					uint64_t a1 = a >> 32;
					uint64_t b0 = b & 0xFFFFFFFF;
					uint64_t b1 = b >> 32;
					uint64_t t1 = a0*b1 + a1*b0;
					uint64_t t1a = t1 << 32;
					int i;

					a0 = a0*b0 + t1a;
					a1 = a1*b1 + (t1 >> 32) + (a0<t1a);
					a0 += r;
					a1 += a0<r;

					for (i = 63; i >= 0; i--) {
							a1 += a1 + ((a0 >> i) & 1);
							t1 += t1;
						if (/*o || */c <= a1) {
								a1 -= c;
								t1++;
					}
				}
					return t1;
			}
			
		}


};

#endif
