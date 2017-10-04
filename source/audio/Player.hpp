#ifndef F1_PLAYER_H
#define F1_PLAYER_H



#include "../utils/AutoPointer.hpp"
#include "../utils/DynamicPointer.hpp"
#include "Sound.hpp"

inline Sample MixSamples2(Sample a, Sample b) {
	return
		// If both samples are negative, mixed signal must have an amplitude between the lesser of A and B, and the minimum permissible negative amplitude
		a < 0 && b < 0 ?
		(a + b) - ((a * b) / SAMPLE_MIN) :

		// If both samples are positive, mixed signal must have an amplitude between the greater of A and B, and the maximum permissible positive amplitude
		(a > 0 && b > 0 ?
		(a + b) - ((a * b) / SAMPLE_MAX)

		// If samples are on opposite sides of the 0-crossing, mixed signal should reflect that samples cancel each other out somewhat
		:
		a + b);
}

struct PlayerItem{
	Sound * sound;
	bool loop;
};

class Player{
private:
	static Player* mInstance;

public:
	AutoPointer<PlayerItem*> mQueue;
	DynamicPointer<Sample> mBuffer;
	Player(){

	}
	static Player * getInstance(){
		if (!mInstance){
			mInstance = new Player();
		}
		return mInstance;
	}
	bool queue(std::string file, bool loop = false){
		Sound* s = new Sound(file);
		if (!s->open())
			return false;
		LOGE("%d", mQueue.size());
		PlayerItem* item = new PlayerItem();
		item->sound = s;
		item->loop = loop;
		mQueue.push(item);
		return true;
	}
	void read(Sample *pointer, int frameCount){
		int samples = frameCount * 2;

		memset(pointer, 0, samples * sizeof(Sample));

		mBuffer.reserve(samples);
		Sample *buffer = mBuffer.getPointer();

		for (int i = 0; i < mQueue.size(); i++){
			PlayerItem* item = mQueue.get(i);
			Sound* sound = item->sound;
			sound->read(buffer, frameCount);
			for (int s = 0; s < samples; s++){
				pointer[s] = MixSamples2(pointer[s], buffer[s]);
			}

			if (sound->getPosition() >= sound->getLength()){
				if (item->loop){
					sound->seek(0);
				}else{
					mQueue.pop(i);
					delete item;
					delete sound;
					sound = NULL;
				}
			}
		}
	}



};

#endif
