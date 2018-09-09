#include "Codec.hpp"
//#include "LibavCodec.h"
//#include "SpotifyCodec.h"
#include "WavCodec.hpp"
#include "Libmpg123Codec.hpp"

#include "../utils/Log.hpp"

#include <ctype.h>


	Codec::Codec(){


	}
	Codec::~Codec(){

	}

	bool Codec::open(std::string source){
		mUri = source;
		return _open();
	}

	std::string Codec::getUri() {
		return mUri;
	}
	int Codec::read(Sample* pointer, int frameCount){

		//return _read(pointer, frameCount);
		/*frameCount = mCache.get(pointer, getPosition() , frameCount);

		if(frameCount < 0){
		//reverse samples
			Sample swap;
			int s = frameCount * getChannels();
			for (int a = 0; a < --s; a++) {
					swap = ((Sample *)pointer)[a];
					((Sample *)pointer)[a] = ((Sample *)pointer)[s];
					((Sample *)pointer)[s] = swap;
			}
		}

		setPosition(getPosition()+frameCount);
		return frameCount;*/
		bool forwards = frameCount > 0;


		if(frameCount == 0 || 
			getPosition() < 0 || 
			(getPosition() <= 0 && !forwards) ||
			getPosition() > getLength() ||
			(getPosition() >= getLength() && forwards)){
			return 0;
		}

		if(getPosition()+frameCount < 0 && getPosition() < abs(frameCount)){
			frameCount = -getPosition();
			
		}else if(getPosition()+frameCount > getLength()){
			frameCount = getLength()-getPosition();
		}



		mReadBuffer.reserve(abs(frameCount) * getFrameSize());
		void *readBuffer = mReadBuffer.getPointer();

		int readed = _read(readBuffer, frameCount);
		int areaded = abs(readed);

		
#if SOUTPUT_FORMAT == SAMPLE_INT16

		if(getBits() == 8){
			//1 Bytes each sample;
			for(int i=0;i<areaded*getChannels();i++){
				pointer[i] = ((((char *)readBuffer)[i] - 128) * 256);
				//pointer[i] = ((((char *)readBuffer)[i] - 128) << 8);
			}		
		}else if(getBits() == 16){
			
			//2 Bytes each sample;
			if(getChannels() == 1){
				for (int i = 0; i < areaded; i++){
					pointer[i * 2] = ((short *)readBuffer)[i];
					pointer[i * 2 + 1] = ((short *)readBuffer)[i];
				}
			}else{

				memcpy(pointer, readBuffer, areaded*getFrameSize());
			}	
			   /*for(int i=0;i<areaded*getChannels();i++){
				pointer[i] = ((short *)readBuffer)[i];
			}*/
		}/*else if(getBits() == 24){
			struct int24{
				signed int data : 24;
			};
			//-8388608 to +8388607
			for(int i=0;i<readed*getChannels();i++){
				pointer[i] = (((short *)readBuffer)[i] >> 8) * 256;
				LOGE("%d", pointer[i]);
			}
		}*/else if(getBits() == 32){
			for(int i=0;i<areaded*getChannels();i++){
				//float f = (((float *)readBuffer)[i]);
				pointer[i] = (short)((((float *)readBuffer)[i]) * SHRT_MAX);
			}	
		}else{
			return 0;
		}

#elif SOUTPUT_FORMAT == SAMPLE_FLOAT32

		//16Short to 32Float
		for(int i=0;i<areaded*getChannels();i++){
			pointer[i] = ((float)((short *)readBuffer)[i] / (float)SHRT_MAX );
		}
#endif



		/*if(readed < 0){
		//reverse samples
			Sample swap;
			int s = areaded * sizeof(Sample);
			for (int a = 0; a < --s; a++) {
					swap = ((Sample *)pointer)[a];
					((Sample *)pointer)[a] = ((Sample *)pointer)[s];
					((Sample *)pointer)[s] = swap;
			}
		}*/


		setPosition(getPosition()+readed);
		return readed;
	}
	void Codec::seek(int frameNumber){
		setPosition(frameNumber);
		_seek(frameNumber);
	}



	bool iequals(std::string a, std::string b){
        unsigned int sz = a.size();
        if (b.size() != sz)
            return false;
        for (unsigned int i = 0; i < sz; ++i)
            if (tolower(a[i]) != tolower(b[i]))
                return false;
        return true;
    }



	Codec* Codec::find(std::string source){
		std::string ext;
		size_t pos = source.find_last_of(".");
		if (pos != std::string::npos){
			ext.assign(source.begin() + pos + 1, source.end());
			Codec* codec = NULL;
			if (iequals(ext, std::string("wav"))){
				codec = new WavCodec();
				if(codec->open(source)){
					return codec;
				}
			}else if(iequals(ext, std::string("mp3")) || iequals(ext, std::string("mp2"))){
				codec = new Libmpg123Codec();
				if(codec->open(source)){
					return codec;
				}
			}
			//codec = new LibavCodec();
			//codec->open(source);
			//return codec;
			//return new SpotifyCodec();
			LOGE("Codec: Failed to load codec to '%s'", source.c_str());
			delete codec;
			
		}
		return NULL;
	}


