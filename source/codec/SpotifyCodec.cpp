#include "SpotifyCodec.hpp"

/*
void print_track(sp_track *track)
{
    int duration = sp_track_duration(track);
    char url[256];
    sp_link *l;
    int i;


    printf("Track %s [%d:%02d] has %d artist(s), %d%% popularity",
           sp_track_name(track),
           duration / 60000,
           (duration / 1000) % 60,
           sp_track_num_artists(track),
           sp_track_popularity(track));

    if(sp_track_disc(track))
        printf(", %d on disc %d",
               sp_track_index(track),
               sp_track_disc(track));
    printf("\n");

    for (i = 0; i < sp_track_num_artists(track); i++) {
        sp_artist *art = sp_track_artist(track, i);
        printf("\tArtist %d: %s\n", i + 1, sp_artist_name(art));
    }
    l = sp_link_create_from_track(track, 0);
    sp_link_as_string(l, url, sizeof(url));
    printf("\t\t%s\n", url);
    sp_link_release(l);
}


SpotifyCodec::SpotifyCodec(){
	sp_error error;
 
	session_callbacks.logged_in = &on_login;
    session_callbacks.notify_main_thread = &on_main_thread_notified;
    session_callbacks.music_delivery = &on_music_delivered;
    session_callbacks.log_message = &on_log;
    session_callbacks.end_of_track = &on_end_of_track;
	session_callbacks.message_to_user = &on_log;
	session_callbacks.start_playback = &on_start_playback;
	session_callbacks.streaming_error = &on_streaming_error;
    // create the spotify session

    spconfig.api_version = SPOTIFY_API_VERSION;
    spconfig.cache_location = "tmp";
    spconfig.settings_location = "tmp";
    spconfig.application_key = g_appkey,
    spconfig.application_key_size = g_appkey_size; // set in main()
    spconfig.user_agent = "spot";
    spconfig.callbacks = &session_callbacks;
	spconfig.userdata = this;


    error = sp_session_create(&spconfig, &mSession);
    if (error != SP_ERROR_OK) {
        LOGE("Error: unable to create spotify session: %s\n", sp_error_message(error));
        return ;
    }
 
    int next_timeout = 0;
 
    sp_session_login(mSession, "angel.arias.gonzalez", "pordefectocrew", 0, NULL);
	do {
		sp_session_process_events(mSession, &next_timeout);
	} while (next_timeout == 0);

	sp_link *link =  sp_link_create_from_string	("spotify:track:20eiLGM6lHj6Ul5dLexnTn");
	sp_track *track = sp_link_as_track	(link) ;

	check:
	do {
		sp_session_process_events(mSession, &next_timeout);
	} while (next_timeout == 0);

	switch (sp_track_error(track)) {
		case SP_ERROR_OK:
			//print_track(track);
			break;
		case SP_ERROR_IS_LOADING:
			Sleep(100);
			goto check; // Still pending
		default:
			LOGE("Unable to resolve track: %s\n", sp_error_message(sp_track_error(track)));
			break;
    }

	error = sp_session_player_load(mSession, track);
	LOGE("%s\n", sp_error_message(error));
	error = sp_track_add_ref(track);
	LOGE("%s\n", sp_error_message(error));
	error = sp_session_player_play(mSession, true);
	LOGE("%s\n", sp_error_message(error));

    //sp_track_release(track);

	//sp_link_release(link);

}
SpotifyCodec::~SpotifyCodec(){

}

bool SpotifyCodec::_load(){
	return false;
}
int SpotifyCodec::_read(void* pointer, int frameCount){
	Lock l(&mMutex);
	int next_timeout = 0;
	do {
		sp_session_process_events(mSession, &next_timeout);
	} while (next_timeout == 0);

	int sn = 0;
	int pos = getPosition();
	int spos = pos*2;
	if(mSamples.size() >= spos){
		for(int i=0;i<frameCount*2;i++){
			if(mSamples.size() < spos+i){
				break;
			}
			((Sample *)pointer)[i] = mSamples[spos+i];
			sn++;
		}
	}
	sn /= 2;
	setPosition(pos+sn);
	return sn;
}
void SpotifyCodec::_seek(unsigned int frameNumber){

}

int __stdcall SpotifyCodec::on_music_delivered(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames){
    //LOGE("callback: on_music_delivered %d %d %d %d", format->channels, format->sample_rate, format->sample_type, num_frames);
	SpotifyCodec *self = (SpotifyCodec *)sp_session_userdata(session);
	self->addSamples((int16_t *)frames, num_frames, format->channels);
    return 0;
}
void SpotifyCodec::addSamples(int16_t * samples, int frames, int channels){
	Lock l(&mMutex);
	for(int i=0;i<frames*channels;i++){
		mSamples.push_back(samples[i]);
	}
	setLength(mSamples.size()/2);
}*/
