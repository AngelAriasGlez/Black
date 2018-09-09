#ifndef _ADJ_CODEC_H
#define _ADJ_CODEC_H

#include "../audio/AudioSource.hpp"
#include "../utils/File.hpp"

#include <string>

#include "../utils/File.hpp"

#include "../utils/DynamicPointer.hpp"





	class Codec: public AudioSource{
	public:

		Codec();
		~Codec();
		static Codec* find(std::string source);

		int read(Sample* pointer, int frameCount);
		void seek(int frameNumber);
		bool open(std::string source);

	//protected:

		virtual int _read(void* pointer, int frameCount) = 0;
		virtual void _seek(int frameNumber) = 0;
		virtual bool _open() = 0;

		/*virtual void _seek(unsigned int frameNumber) = 0;
		virtual unsigned int _read(short* pointer, unsigned int frameCount) = 0;*/

		std::string getUri();
		std::string mUri;
	private:
		int mConverterBufferSize;
		void* mConverterBuffer;
		DynamicPointer<char> mReadBuffer;


	};

#endif
