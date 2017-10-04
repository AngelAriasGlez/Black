
/*#include "Codec.h"
#include <stdint.h>
#include <vector>

#include "libspotify/api.h"

#include "thread/Mutex.h"
#include "thread/Lock.h"

static void __stdcall on_login(sp_session *session, sp_error error){
    if (error != SP_ERROR_OK) {
        LOGE("Spotify: unable to log in: %s\n", sp_error_message(error));
	}else{
		LOGI("Spotify: Logged");
	} 
}
 
static void __stdcall on_main_thread_notified(sp_session *session){
    //LOGE("callback: on_main_thread_notified");
}
 
static void __stdcall on_log(sp_session *session, const char *data){
    // this method is *very* verbose, so this data should really be written out to a log file
	LOGI("Spotify: %s", data);
}
 
static void __stdcall on_end_of_track(sp_session *session){
    LOGE("Spotify:  on_end_of_track");
}

static void __stdcall on_start_playback(sp_session *session){
    LOGE("Spotify: Start playback");
}
static void __stdcall on_streaming_error(sp_session *session, sp_error error){
    LOGE("Spotify: Streaming error %s\n", sp_error_message(error));
}

static sp_session_callbacks session_callbacks;
static sp_session_config spconfig;


const uint8_t g_appkey[] = {
	0x01, 0xBB, 0x80, 0x7C, 0x0C, 0xE3, 0x2D, 0x44, 0xA4, 0x40, 0x37, 0xAC, 0x34, 0xC4, 0xEE, 0x99,
	0x5E, 0x37, 0x85, 0x94, 0x97, 0x98, 0x81, 0x02, 0xEC, 0x35, 0x3F, 0xFD, 0xFF, 0xE9, 0x81, 0x60,
	0x40, 0x56, 0x67, 0x42, 0x66, 0x88, 0xCF, 0xFA, 0xA5, 0x27, 0x07, 0x52, 0x7A, 0xCB, 0x06, 0x44,
	0xEE, 0xF2, 0x80, 0xE7, 0xEB, 0xC9, 0xA0, 0x38, 0x90, 0x57, 0x5E, 0x30, 0xBB, 0xC6, 0x21, 0x31,
	0x3D, 0xE8, 0x34, 0xC7, 0xBA, 0x3D, 0xB1, 0x43, 0x90, 0x6D, 0x23, 0xD8, 0x02, 0xBA, 0x5F, 0x3C,
	0x54, 0xD0, 0x3B, 0xF1, 0xF0, 0xE2, 0x68, 0xDB, 0xFC, 0xF3, 0xFE, 0x08, 0x50, 0x5D, 0x39, 0x98,
	0xBF, 0x22, 0xD8, 0x03, 0xFB, 0x12, 0x8A, 0x72, 0x17, 0xB9, 0xDC, 0x92, 0x11, 0x65, 0xFC, 0xF5,
	0xB9, 0xE6, 0xE2, 0x34, 0x03, 0x24, 0x7E, 0xD9, 0x33, 0xB9, 0x78, 0x93, 0xD4, 0xA1, 0x31, 0x02,
	0xF8, 0x34, 0x4F, 0x04, 0xF7, 0xEB, 0xCC, 0x84, 0x99, 0x87, 0xC8, 0x50, 0x8F, 0x60, 0x50, 0x91,
	0xCC, 0x1C, 0xF7, 0x4D, 0x14, 0x34, 0xB8, 0xF5, 0x4B, 0x95, 0x10, 0xE0, 0x31, 0x11, 0x98, 0xFD,
	0x70, 0x6A, 0x4D, 0xFA, 0xD6, 0xC7, 0xA8, 0xDD, 0xFB, 0x4D, 0x2D, 0xD1, 0x12, 0x15, 0x1C, 0x8C,
	0x6F, 0x5C, 0x38, 0x44, 0x81, 0xCE, 0xA6, 0x7C, 0x46, 0x4E, 0xA9, 0x33, 0x6E, 0x77, 0x0F, 0x4B,
	0x97, 0x90, 0x6E, 0x31, 0x7A, 0x17, 0x04, 0x7E, 0x8B, 0x31, 0x6C, 0x9B, 0x9B, 0xCF, 0x34, 0xC2,
	0xDD, 0x80, 0x99, 0x66, 0x0F, 0xB9, 0xE2, 0x42, 0xEA, 0xD0, 0x41, 0x9C, 0x03, 0x94, 0xA9, 0x87,
	0x79, 0xF9, 0x4C, 0xBF, 0xE0, 0x64, 0x32, 0x66, 0x66, 0xA2, 0x5D, 0x9D, 0xEF, 0x7C, 0xB8, 0x7A,
	0xDA, 0x51, 0x2F, 0x70, 0xB0, 0xC7, 0x49, 0xBE, 0xD3, 0x73, 0x2B, 0x9F, 0xAD, 0x6A, 0x30, 0x05,
	0x93, 0x02, 0x56, 0xA7, 0xB8, 0x6D, 0x2F, 0x13, 0x7E, 0xE2, 0x1B, 0xE3, 0x7D, 0x34, 0x9E, 0x18,
	0x74, 0xE6, 0x48, 0xE0, 0x88, 0x61, 0x08, 0x71, 0x3F, 0xB9, 0x51, 0x2F, 0x95, 0x41, 0xCC, 0x14,
	0x57, 0xB8, 0x2C, 0xFC, 0x84, 0x4A, 0xEC, 0x59, 0xBD, 0xE3, 0x13, 0xF2, 0x75, 0xB3, 0xB7, 0xAF,
	0x3C, 0x12, 0xC0, 0xC7, 0x3D, 0x8D, 0x2B, 0xFB, 0x42, 0x07, 0x0D, 0xD2, 0xDE, 0x8A, 0x6A, 0x4B,
	0x16,
};
const size_t g_appkey_size = sizeof(g_appkey);



class SpotifyCodec : public Codec{
private:
	sp_session *mSession;

	vector<int16_t> mSamples;

	Mutex mMutex;
public:
	SpotifyCodec();
	~SpotifyCodec();

	bool _load();
	int _read(void* pointer, int frameCount);
	void _seek(unsigned int frameNumber);

	static int __stdcall on_music_delivered(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames);
	void addSamples(int16_t * samples, int frames, int channels);

};*/