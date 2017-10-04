#ifndef _FSOUND_H
#define _FSOUND_H


#include <string>




#include "AudioSource.hpp"
#include "../codec/Codec.hpp"


class Sound: public AudioSource{
	public:
		Sound();
		Sound(String file);
		virtual ~Sound();

		void init();

		void setUri(String uri);

		virtual bool open();


		virtual int read(Sample* pointer, int frameCount);
		void seek(int frameNumber);

		Codec* getCodec(){

			return _Codec;
		};
		bool isOpen() {
			return !(_Codec == NULL);
		}

		String getUri();


	private:
		String mUri;

		Codec* _Codec;





	};

#endif
