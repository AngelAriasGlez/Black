#include "Sound.hpp"




#include "../codec/WavCodec.hpp"
#include "../codec/Mp3Codec.hpp"


Sound::Sound(){
	init();
}

Sound::Sound(String uri){
	init();

	setUri(uri);
}
void Sound::setUri(String uri){
	mUri = uri;
}
String Sound::getUri(){
	//Lock l(&mOpenMutex);
	return mUri;
}
Sound::~Sound(){
	if(_Codec){
        delete _Codec;
    }
}
void Sound::init(){
	_Codec = NULL;
}


bool Sound::open(){
	//Lock l(&mOpenMutex);
	_Codec = Codec::find(mUri);
	if (!_Codec) {
		return false;
	}


	/*const int chunk = 44100 * 0.01161 + 0.0001;
	Sample pointer[chunk*2];
	TempoAnalyzer *t = new TempoAnalyzer(chunk, getSamplerate(), getChannels());

	int totalRead = 0;
	int pd = 0;
	while(true){
		double prog = (100.0 / (double)_Codec->getLength() * (double)totalRead);
		if(prog >= pd){
			LOGI("%d%%", (int)prog);
			pd += 10;
		}
		int read = _Codec->read(pointer, chunk);
		if(read == 0) break;
		totalRead +=read;
		t->process(pointer);

		if(prog > 20) break;
	}
	t->retrieve();*/

	setBits(_Codec->getBits());
	setChannels(_Codec->getChannels());
	setLength(_Codec->getLength());
	setSamplerate(_Codec->getSamplerate());

    
    return true;
}

int Sound::read(Sample* pointer, int frameCount){
	//Lock l(&mOpenMutex);
	if(_Codec){
		//mCache->setSource(_Codec);
		int readed = _Codec->read(pointer, frameCount);
		setPosition(_Codec->getPosition());

		return readed;
	}else
		return 0;
}
void Sound::seek(int frameNumber){
	//Lock l(&mOpenMutex);
	if(_Codec){
		_Codec->seek(frameNumber);
		setPosition(_Codec->getPosition());
	}
}
