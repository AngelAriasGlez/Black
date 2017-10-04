//#define __ANDROID__
#ifdef __ANDROID__

#include "../utils/Log.h"

#include "Platform.h"

#include <unistd.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <android/configuration.h>

#include <string>
#include "Application.h"
#include <cmath>

#define WINDOW_VSYNC 0

// Externally referenced global variables.
struct android_app* __state;
AAssetManager* __assetManager;

static bool __initialized;
static bool __suspended;
static EGLDisplay __eglDisplay = EGL_NO_DISPLAY;
static EGLContext __eglContext = EGL_NO_CONTEXT;
static EGLSurface __eglSurface = EGL_NO_SURFACE;
static EGLConfig __eglConfig = 0;
static int __width;
static int __height;

static bool __vsync = WINDOW_VSYNC;

static bool __multiSampling = false;

static std::string __cacheDir;

// OpenGL VAO functions.
static const char* __glExtensions;
/*PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays = NULL;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = NULL;
PFNGLISVERTEXARRAYOESPROC glIsVertexArray = NULL;*/



// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static SLEffectSendItf bqPlayerEffectSend;
static SLMuteSoloItf bqPlayerMuteSolo;
static SLVolumeItf bqPlayerVolume;


static const char * opensl_get_error_string(SLresult result)
{
	switch (result){
	case SL_RESULT_PRECONDITIONS_VIOLATED: return "Preconditions violated";
	case SL_RESULT_PARAMETER_INVALID: return "Invalid parameter";
	case SL_RESULT_MEMORY_FAILURE: return "Memory failure";
	case SL_RESULT_RESOURCE_ERROR: return "Resource error";
	case SL_RESULT_RESOURCE_LOST: return "Resource lost";
	case SL_RESULT_IO_ERROR: return "IO error";
	case SL_RESULT_BUFFER_INSUFFICIENT: return "Insufficient buffer";
	case SL_RESULT_CONTENT_CORRUPTED: return "Content corrupted";
	case SL_RESULT_CONTENT_UNSUPPORTED: return "Content unsupported";
	case SL_RESULT_CONTENT_NOT_FOUND: return "Content not found";
	case SL_RESULT_PERMISSION_DENIED: return "Permission denied";
	case SL_RESULT_FEATURE_UNSUPPORTED: return "Feature unsupported";
	case SL_RESULT_INTERNAL_ERROR: return "Internal error";
	case SL_RESULT_UNKNOWN_ERROR: return "Unknown error";
	case SL_RESULT_OPERATION_ABORTED: return "Operation aborted";
	case SL_RESULT_CONTROL_LOST: return "Control lost";
	}
	return "Unknown";
}

void checkSL( SLresult res )
{
	if ( res != SL_RESULT_SUCCESS )
	{
		// Debug printing to be placed here
		LOGE("OpenSL Error: %s",opensl_get_error_string(res));
	}
}


static void* buffer[1024];
static short sbuffer[1024];

// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	//assert(bq == bqPlayerBufferQueue);
	//assert(NULL == context);
	// for streaming playback, replace this test by logic to find and fill the next buffer
	SLresult result;
	// enqueue another buffer
	Application::getInstance()->soundStreamInternal(buffer, 256);

	for(int i=0;i<256*2;i++){
		sbuffer[i] = (short)((((float *)buffer)[i]) * SHRT_MAX);
	}

	result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, sbuffer, 256*sizeof(short)*2);
	// the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
	// which for this code example would indicate a programming error
	//checkSL(SL_RESULT_SUCCESS == result);
}


// create the engine and output mix objects
void initializeSound()
{
	SLresult result;

	// create engine
	SLEngineOption engineOption[] = {SL_ENGINEOPTION_THREADSAFE, SL_BOOLEAN_TRUE};
	result = slCreateEngine(&engineObject, 1, engineOption, 0, NULL, NULL);
	checkSL(result);

	// realize the engine
	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	checkSL(result);

	// get the engine interface, which is needed in order to create other objects
	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
	checkSL(result);


	// create output mix, with environmental reverb specified as a non-required interface
	//const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
	//const SLboolean req[1] = {SL_BOOLEAN_FALSE};
	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
	checkSL(result);

	// realize the output mix
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	checkSL(result);


	// configure audio source
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
	SLDataFormat_PCM pcm;
	pcm.formatType = SL_DATAFORMAT_PCM;
	pcm.numChannels = 2;
	pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
	pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
	pcm.containerSize = pcm.bitsPerSample;
	pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
	pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

	SLDataSource audioSrc = {&loc_bufq, &pcm};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	SLDataSink audioSnk = {&loc_outmix, NULL};

	// create audio player
	const SLInterfaceID ids[3] = {
		SL_IID_BUFFERQUEUE,
		SL_IID_VOLUME
	};
	const SLboolean req[3] = {
		SL_BOOLEAN_TRUE,
		SL_BOOLEAN_TRUE
	};
    
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, 2, ids, req);
	checkSL(result);

	// realize the player
	result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	checkSL(result);

	// get the play interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
	checkSL(result);

	// get the buffer queue interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
	checkSL(result);

	// register callback on the buffer queue
	result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
	checkSL(result);

	// get the volume interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
	checkSL(result);


	// set the player's state to playing
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	checkSL(result);


	Application::getInstance()->soundStreamInternal(buffer, 512);
	int bytes = 512*4;
    

	result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer, bytes);
	checkSL(result);

}
// shut down the native audio system
void shutdownSound()
{

	// destroy buffer queue audio player object, and invalidate all associated interfaces
	if (bqPlayerObject != NULL) {
		(*bqPlayerObject)->Destroy(bqPlayerObject);
		bqPlayerObject = NULL;
		bqPlayerPlay = NULL;
		bqPlayerBufferQueue = NULL;
		bqPlayerEffectSend = NULL;
		bqPlayerMuteSolo = NULL;
		bqPlayerVolume = NULL;
	}

	// destroy output mix object, and invalidate all associated interfaces
	if (outputMixObject != NULL) {
		(*outputMixObject)->Destroy(outputMixObject);
		outputMixObject = NULL;
		outputMixEnvironmentalReverb = NULL;
	}

	// destroy engine object, and invalidate all associated interfaces
	if (engineObject != NULL) {
		(*engineObject)->Destroy(engineObject);
		engineObject = NULL;
		engineEngine = NULL;
	}
}















	static EGLenum checkErrorEGL(const char* msg)
	{
		//ASSERT(msg);
		static const char* errmsg[] =
		{
			"EGL function succeeded",
			"EGL is not initialized, or could not be initialized, for the specified display",
			"EGL cannot access a requested resource",
			"EGL failed to allocate resources for the requested operation",
			"EGL fail to access an unrecognized attribute or attribute value was passed in an attribute list",
			"EGLConfig argument does not name a valid EGLConfig",
			"EGLContext argument does not name a valid EGLContext",
			"EGL current surface of the calling thread is no longer valid",
			"EGLDisplay argument does not name a valid EGLDisplay",
			"EGL arguments are inconsistent",
			"EGLNativePixmapType argument does not refer to a valid native pixmap",
			"EGLNativeWindowType argument does not refer to a valid native window",
			"EGL one or more argument values are invalid",
			"EGLSurface argument does not name a valid surface configured for rendering",
			"EGL power management event has occurred",
		};
		EGLenum error = eglGetError();
		LOGE("%s: %s.", msg, errmsg[error - EGL_SUCCESS]);
		return error;
	}


	// Initialized EGL resources.
	static bool initEGL()
	{
		int samples = 2;

		// Hard-coded to 32-bit/OpenGL ES 2.0.
		// NOTE: EGL_SAMPLE_BUFFERS, EGL_SAMPLES and EGL_DEPTH_SIZE MUST remain at the beginning of the attribute list
		// since they are expected to be at indices 0-5 in config fallback code later.
		// EGL_DEPTH_SIZE is also expected to
		EGLint eglConfigAttrs[] =
		{
			EGL_SAMPLE_BUFFERS, samples > 0 ? 1 : 0,
			EGL_SAMPLES, samples,
			EGL_DEPTH_SIZE, 24,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_STENCIL_SIZE, 8,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_NONE
		};
		__multiSampling = samples > 0;

		EGLint eglConfigCount;
		const EGLint eglContextAttrs[] =
		{
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
		};

		const EGLint eglSurfaceAttrs[] =
		{
			EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
			EGL_NONE
		};

		if (__eglDisplay == EGL_NO_DISPLAY && __eglContext == EGL_NO_CONTEXT)
		{
			// Get the EGL display and initialize.
			__eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			if (__eglDisplay == EGL_NO_DISPLAY)
			{
				checkErrorEGL("eglGetDisplay");
				goto error;
			}

			if (eglInitialize(__eglDisplay, NULL, NULL) != EGL_TRUE)
			{
				checkErrorEGL("eglInitialize");
				goto error;
			}

			// Try both 24 and 16-bit depth sizes since some hardware (i.e. Tegra) does not support 24-bit depth
			bool validConfig = false;
			EGLint depthSizes[] = { 24, 16 };
			for (unsigned int i = 0; i < 2; ++i)
			{
				eglConfigAttrs[1] = samples > 0 ? 1 : 0;
				eglConfigAttrs[3] = samples;
				eglConfigAttrs[5] = depthSizes[i];

				if (eglChooseConfig(__eglDisplay, eglConfigAttrs, &__eglConfig, 1, &eglConfigCount) == EGL_TRUE && eglConfigCount > 0)
				{
					validConfig = true;
					break;
				}

				if (samples)
				{
					// Try lowering the MSAA sample size until we find a  config
					int sampleCount = samples;
					while (sampleCount)
					{
						LOGW("No EGL config found for depth_size=%d and samples=%d. Trying samples=%d instead.", depthSizes[i], sampleCount, sampleCount / 2);
						sampleCount /= 2;
						eglConfigAttrs[1] = sampleCount > 0 ? 1 : 0;
						eglConfigAttrs[3] = sampleCount;
						if (eglChooseConfig(__eglDisplay, eglConfigAttrs, &__eglConfig, 1, &eglConfigCount) == EGL_TRUE && eglConfigCount > 0)
						{
							validConfig = true;
							break;
						}
					}

					__multiSampling = sampleCount > 0;

					if (validConfig)
						break;
				}
				else
				{
					LOGW("No EGL config found for depth_size=%d.", depthSizes[i]);
				}
			}

			if (!validConfig)
			{
				checkErrorEGL("eglChooseConfig");
				goto error;
			}

			__eglContext = eglCreateContext(__eglDisplay, __eglConfig, EGL_NO_CONTEXT, eglContextAttrs);
			if (__eglContext == EGL_NO_CONTEXT)
			{
				checkErrorEGL("eglCreateContext");
				goto error;
			}
		}

		// EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
		// guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
		// As soon as we picked a EGLConfig, we can safely reconfigure the
		// ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
		EGLint format;
		eglGetConfigAttrib(__eglDisplay, __eglConfig, EGL_NATIVE_VISUAL_ID, &format);
		ANativeWindow_setBuffersGeometry(__state->window, 0, 0, format);

		__eglSurface = eglCreateWindowSurface(__eglDisplay, __eglConfig, (EGLNativeWindowType)__state->window, eglSurfaceAttrs);
		if (__eglSurface == EGL_NO_SURFACE)
		{
			checkErrorEGL("eglCreateWindowSurface");
			goto error;
		}

		if (eglMakeCurrent(__eglDisplay, __eglSurface, __eglSurface, __eglContext) != EGL_TRUE)
		{
			checkErrorEGL("eglMakeCurrent");
			goto error;
		}

		eglQuerySurface(__eglDisplay, __eglSurface, EGL_WIDTH, &__width);
		eglQuerySurface(__eglDisplay, __eglSurface, EGL_HEIGHT, &__height);


		// Set vsync.
		eglSwapInterval(__eglDisplay, WINDOW_VSYNC ? 1 : 0);

		// Initialize OpenGL ES extensions.
		__glExtensions = (const char*)glGetstd::string(GL_EXTENSIONS);

		/*if (strstr(__glExtensions, "GL_OES_vertex_array_object") || strstr(__glExtensions, "GL_ARB_vertex_array_object"))
		{
			// Disable VAO extension for now.
			glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
			glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
			glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
			glIsVertexArray = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress("glIsVertexArrayOES");
		}*/

		return true;

	error:
		return false;
	}

	static void destroyEGLSurface()
	{
		if (__eglDisplay != EGL_NO_DISPLAY)
		{
			eglMakeCurrent(__eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		}

		if (__eglSurface != EGL_NO_SURFACE)
		{
			eglDestroySurface(__eglDisplay, __eglSurface);
			__eglSurface = EGL_NO_SURFACE;
		}
	}

	static void destroyEGLMain()
	{
		destroyEGLSurface();

		if (__eglContext != EGL_NO_CONTEXT)
		{
			eglDestroyContext(__eglDisplay, __eglContext);
			__eglContext = EGL_NO_CONTEXT;
		}

		if (__eglDisplay != EGL_NO_DISPLAY)
		{
			eglTerminate(__eglDisplay);
			__eglDisplay = EGL_NO_DISPLAY;
		}
	}


	// Process the next input event.
	static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
	{

		if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
		{
			int32_t action = AMotionEvent_getAction(event);

			int pointerIndex;
			int pointerCount = AMotionEvent_getPointerCount(event);
			int x;
			int y;
			for(int pointerId = 0;pointerId<pointerCount;pointerId++){
				  x = AMotionEvent_getX(event, pointerId);
				  y = AMotionEvent_getY(event, pointerId);
				  TouchEvent e;
				  e.x = x;
				  e.y = y;
				  e.id = AMotionEvent_getPointerId(event, pointerId);

			switch (action & AMOTION_EVENT_ACTION_MASK)
			{
			case AMOTION_EVENT_ACTION_DOWN:
			{
				struct mallinfo info = mallinfo();
				//LOGE("Size %d, Allocated %d, Free %d", info.usmblks, info.uordblks, info.fordblks);
				e.type = TouchType::DOWN;							  // Primary pointer down.
                Application::getInstance()->touchEventInternal(e, pointerCount);

			}
				break;

			case AMOTION_EVENT_ACTION_UP:
			{
				e.type = TouchType::UP;
				Application::getInstance()->touchEventInternal(e, pointerCount);
			}
				break;

			case AMOTION_EVENT_ACTION_POINTER_DOWN:
			{

			}
				break;

			case AMOTION_EVENT_ACTION_POINTER_UP:
			{

			}
				break;

			case AMOTION_EVENT_ACTION_MOVE:
			{
				e.type = TouchType::MOVE;
				Application::getInstance()->touchEventInternal(e, pointerCount);

			}
				break;
			}
			}
			return 1;
		}
		else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
		{
			int32_t action = AKeyEvent_getAction(event);
			int32_t keycode = AKeyEvent_getKeyCode(event);
			int32_t metastate = AKeyEvent_getMetaState(event);

			//Don't consume volume up/down events.
			if (keycode == AKEYCODE_VOLUME_DOWN || keycode == AKEYCODE_VOLUME_UP)
				return 0;

			switch (action)
			{
			case AKEY_EVENT_ACTION_DOWN:

				break;

			case AKEY_EVENT_ACTION_UP:

				break;
			}

			return 1;
		}
		return 0;
	}

	// Process the next main command.
	static void engine_handle_cmd(struct android_app* app, int32_t cmd)
	{
		switch (cmd)
		{
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			if (app->window != NULL)
			{
				initEGL();
				__initialized = true;
			}
			break;
		case APP_CMD_TERM_WINDOW:
			destroyEGLSurface();
			__initialized = false;
			break;
		case APP_CMD_DESTROY:
			Application::getInstance()->shutdown();
			destroyEGLMain();
			__initialized = false;
			break;
		case APP_CMD_GAINED_FOCUS:
			if (Application::getInstance()->getState() != Application::RUNNING){
				Application::getInstance()->run();
			}
			break;
		case APP_CMD_RESUME:
			break;
		case APP_CMD_PAUSE:

			break;
		case APP_CMD_LOST_FOCUS:

			break;
		}
	}

	Platform::Platform()
	{
	}

	Platform::~Platform()
	{
	}

	bool Platform::create()
	{

	}

	int Platform::start()
	{
		Application *app = Application::getInstance();
		//ASSERT(__state && __state->activity && __state->activity->vm);

		__initialized = false;
		__suspended = false;

		// Get the android application's activity.
		ANativeActivity* activity = __state->activity;
		JavaVM* jvm = __state->activity->vm;
		JNIEnv* env = NULL;
		jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
		jint res = jvm->AttachCurrentThread(&env, NULL);
		if (res == JNI_ERR)
		{
			LOGE("Failed to retrieve JVM environment when entering message pump.");
			return -1;
		}
		//ASSERT(env);

		/* Get external files directory on Android; this will result in a directory where all app files
		* should be stored, like /mnt/sdcard/android/<package-name>/files/
		*/
		jboolean isCopy;

		jclass clazz = env->GetObjectClass(activity->clazz);
		jmethodID methodGetExternalStorage = env->GetMethodID(clazz, "getExternalFilesDir", "(Ljava/lang/std::string;)Ljava/io/File;");

		jclass clazzFile = env->FindClass("java/io/File");
		jmethodID methodGetPath = env->GetMethodID(clazzFile, "getPath", "()Ljava/lang/std::string;");

		// Now has java.io.File object pointing to directory
		jobject objectFile = env->CallObjectMethod(activity->clazz, methodGetExternalStorage, NULL);

		// Now has std::string object containing path to directory
		jstring stringExternalPath = static_cast<jstring>(env->CallObjectMethod(objectFile, methodGetPath));
		const char* externalPath = env->Getstd::stringUTFChars(stringExternalPath, &isCopy);

		// Set the default path to store the resources.
		std::string assetsPath(externalPath);
		if (externalPath[strlen(externalPath) - 1] != '/')
			assetsPath += "/";

		__cacheDir = assetsPath;

		//FileSystem::setResourcePath(assetsPath.c_str());

		// Release string data
		env->Releasestd::stringUTFChars(stringExternalPath, externalPath);
		jvm->DetachCurrentThread();

		// Get the asset manager to get the resources from the .apk file.
		__assetManager = activity->assetManager;

		// Set the event call back functions.
		__state->onAppCmd = engine_handle_cmd;
		__state->onInputEvent = engine_handle_input;


		initializeSound();
        while (true)
		{
			// Read all pending events.
			int ident;
			int events;
			struct android_poll_source* source;

			while ((ident = ALooper_pollAll(!__suspended ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
			{
				// Process this event.
				if (source != NULL)
					source->process(__state, source);

				if (__state->destroyRequested != 0)
				{
					return 0;
				}
			}

			// Idle time (no events left to process) is spent rendering.
			// We skip rendering when the app is paused.
			if (__initialized && !__suspended)
			{
				app->frame();

				// Post the new frame to the display.
				// Note that there are a couple cases where eglSwapBuffers could fail
				// with an error code that requires a certain level of re-initialization:
				//
				// 1) EGL_BAD_NATIVE_WINDOW - Called when the surface we're currently using
				//    is invalidated. This would require us to destroy our EGL surface,
				//    close our OpenKODE window, and start again.
				//
				// 2) EGL_CONTEXT_LOST - Power management event that led to our EGL context
				//    being lost. Requires us to re-create and re-initalize our EGL context
				//    and all OpenGL ES state.
				//
				// For now, if we get these, we'll simply exit.
				int rc = eglSwapBuffers(__eglDisplay, __eglSurface);
				if (rc != EGL_TRUE)
				{
					EGLint error = eglGetError();
					if (error == EGL_BAD_NATIVE_WINDOW)
					{
						if (__state->window != NULL)
						{
							destroyEGLSurface();
							initEGL();
						}
						__initialized = true;
					}
					else
					{
						perror("eglSwapBuffers");
						break;
					}
				}
			}

			// Display the keyboard.
			//gameplay::displayKeyboard(__state, __displayKeyboard);
		}
	}

	void Platform::shutdown()
	{
		shutdownSound();
		// nothing to do  
	}


	unsigned int Platform::getDisplayWidth()
	{
		return __width;
	}

	unsigned int Platform::getDisplayHeight()
	{
		return __height;
	}


	double Platform::getDisplaySize(){
		double density = getDisplayDensity();
		double x = pow(__width/density,2);
		double y = pow(__height/density,2);
		double screenInches = sqrt(x+y);
		LOGE("%f inches", screenInches);
		return screenInches;
	}
	double Platform::getDisplayDensity(){
		AConfiguration* ac = AConfiguration_new();
		AConfiguration_fromAssetManager(ac, __assetManager);
		double density = AConfiguration_getDensity(ac);
		AConfiguration_delete(ac);
		return density;
	}
	std::string Platform::getCacheDir(){
		return std::string("sdcard/kuartz/cache/");
		//return __cacheDir;
	}
	std::string Platform::getAssetsDir(){
		return std::string("sdcard/kuartz/");
	}

	std::string Platform::displayFileDialog(const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
	{
		return "/sdcard/Klangkarussell - Sonnentanz (Oliver Koletzki Remix).mp3";
	}



#endif
