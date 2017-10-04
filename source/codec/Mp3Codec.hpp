#ifndef _ADJ_MP3CODEC_H
#define _ADJ_MP3CODEC_H

#include <algorithm>


#include "../utils/File.hpp"
#include "Codec.hpp"

#include <vector>
extern "C"{
#include "mpaudec/mpaudec.h"
}

#include "../utils/QueueBuffer.hpp"





	class Mp3Codec : public Codec{
	public:


		Mp3Codec();
		~Mp3Codec();
		
		bool _open();
		int _read(void* pointer, int frameCount);
		void _seek(int frameNumber);

	private:
		bool decodeFrame();

		MPAuDecContext* mpauContext;
		unsigned char* mpauOutputBuffer;
		unsigned char* mpauInputBuffer;

		QueueBuffer* mBuffer;

		bool mEof;

		int mInputPosition;
		int mInputLength;
		int mPosition;
		std::vector<int> mFrameSizes;
		std::vector<int> mFrameOffsets;

		void reset();

		bool ID3v2Match(unsigned char* buf);

		int skipFrames;

		FileMap *mFile;
	};


#endif



