#ifdef __ANDROID__

#include <android_native_app_glue.h>
#include "Platform.h"

#include "../utils/Log.h"

extern struct android_app* __state;

/**
* Main entry point.
*/
void android_main(struct android_app* state)
{
	// Android specific : Dummy function that needs to be called to 
	// ensure that the native activity works properly behind the scenes.
	app_dummy();

	__state = state;

	if (Platform::create()){
		Platform::start();
	}

	// Android specific : the process needs to exit to trigger
	// cleanup of global and static resources (such as the game).
	exit(0);
}

#endif
