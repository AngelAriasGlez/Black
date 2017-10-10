#if defined(_WIN64) | defined(_WIN32)
#include "Platform.hpp"
#include "Application.hpp"
#include "../utils/Utils.hpp"
#include "../preferences/Preference.hpp"
#include "Shlwapi.h"

#include "../utils/Log.hpp"

#define NANOVG_GL3_IMPLEMENTATION

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>
#pragma comment (lib, "opengl32.lib")

#include <windowsx.h>


//#define DEFAULT_RESOLUTION_X 2880
//#define DEFAULT_RESOLUTION_Y 1800

#define DEFAULT_RESOLUTION_X 750
#define DEFAULT_RESOLUTION_Y 1334
#define DEFAULT_COLOR_BUFFER_SIZE 32
#define DEFAULT_DEPTH_BUFFER_SIZE 24
#define DEFAULT_STENCIL_BUFFER_SIZE 8

#include <sstream>
#include <string>

#define WINDOW_VSYNC        0

static bool __vsync = WINDOW_VSYNC;
static HINSTANCE __hinstance = 0;
static HWND __hwnd = 0;
static HDC __hdc = 0;
static HGLRC __hrc = 0;

static bool __multiSampling = false;
static Platform::AudioInfo __audioInfo;


#include <portaudio.h>
#include <pa_asio.h>
PaStream* __soundStream;


#ifdef USE_MIDI
#include "rtmidi/RtMidi.h"
#endif

#include <Commdlg.h>

#include <map>


#define SAMPLE_INT16 33
#define SAMPLE_FLOAT32 44

#define OUTPUT_FORMAT SAMPLE_FLOAT32

struct WindowCreationParams
{
	RECT rect;
	std::string windowName;
	bool fullscreen;
	bool resizable;
	int samples;
};


int soundStreamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData){
	Application::getInstance()->soundStreamInternal(outputBuffer, framesPerBuffer);
	return 0;
}
void midiStreamCallback(double deltatime, std::vector< unsigned char > *message, void *userData){
	Application::getInstance()->midiStreamInternal(deltatime, message);
	/*unsigned int nBytes = message->size();
	for (unsigned int i = 0; i<nBytes; i++)
		std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
	if (nBytes > 0)
		std::cout << "stamp = " << deltatime << std::endl;*/
}

#ifdef USE_MIDI
bool initializeMidi(){
	// Create an api map.
	std::map<int, std::string> apiMap;
	apiMap[RtMidi::MACOSX_CORE] = "OS-X CoreMidi";
	apiMap[RtMidi::WINDOWS_MM] = "Windows MultiMedia";
	apiMap[RtMidi::UNIX_JACK] = "Jack Client";
	apiMap[RtMidi::LINUX_ALSA] = "Linux ALSA";
	apiMap[RtMidi::RTMIDI_DUMMY] = "RtMidi Dummy";

	std::vector< RtMidi::Api > apis;
	RtMidi :: getCompiledApi( apis );

	std::cout << "\nCompiled APIs:\n";
	for ( unsigned int i=0; i<apis.size(); i++ )
	std::cout << "  " << apiMap[ apis[i] ] << std::endl;

	RtMidiIn *midiin = 0;

	// RtMidiIn constructor
	try {
	midiin = new RtMidiIn();
	}
	catch ( RtMidiError &error ) {
	error.printMessage();
	exit( EXIT_FAILURE );
	}

	// Check inputs.
	unsigned int nPorts = midiin->getPortCount();
	std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
	std::string portName;
	for ( unsigned int i=0; i<nPorts; i++ ) {
		try {
			portName = midiin->getPortName(i);
		}catch (RtMidiError &error) {
			error.printMessage();
			// goto cleanup;
		}
		std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
	}

	if (nPorts >= 1){
		midiin->openPort(0);

		// Set our callback function.  This should be done immediately after
		// opening the port to avoid having incoming messages written to the
		// queue.
		midiin->setCallback(&midiStreamCallback);

		// Don't ignore sysex, timing, or active sensing messages.
		midiin->ignoreTypes(false, true, false);

	}
	/*std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
	char input;
	std::cin.get(input);

	// Clean up
	cleanup:
	delete midiin;*/


}
#endif

PaStream* initializeSoundStream(int deviceindex) {
	__audioInfo.defaultDevice = deviceindex;

	PaStream* stream;
	PaStreamParameters outputParameters;
	outputParameters.device = deviceindex;
	outputParameters.channelCount = 2;       // MONO output 

	if (OUTPUT_FORMAT == SAMPLE_FLOAT32) {
		outputParameters.sampleFormat = paFloat32;
	}
	else {
		outputParameters.sampleFormat = paInt16;
	}


	PaAsioStreamInfo asioOutputInfo;
	// Use an ASIO specific structure. WARNING - this is not portable.
	asioOutputInfo.size = sizeof(PaAsioStreamInfo);
	asioOutputInfo.hostApiType = paASIO;
	asioOutputInfo.version = 1;
	asioOutputInfo.flags = paAsioUseChannelSelectors;
	int outputChannelSelectors[2];
	outputChannelSelectors[0] = 0; // skip channel 0 and use the second (right) ASIO device channel
	outputChannelSelectors[1] = 1; // skip channel 0 and use the second (right) ASIO device channel
	asioOutputInfo.channelSelectors = outputChannelSelectors;

	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	//outputParameters.hostApiSpecificStreamInfo = &asioOutputInfo;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	PaError err = Pa_OpenStream(&stream, 0, &outputParameters, 44100, 64, paClipOff, soundStreamCallback, 0);
	if (err != paNoError) {

		LOGE("Error number: %d\n", err);
		LOGE("Error message: %s\n", Pa_GetErrorText(err));
		return NULL;
	}

	err = Pa_StartStream(stream);
	if (err != paNoError) {
		Pa_CloseStream(stream);
		return NULL;

	}
	return stream;
}



PaStream* initializeSound(){

	Pa_Initialize();

	int numDevices = Pa_GetDeviceCount();
	const   PaDeviceInfo *deviceInfo;
	for (int i = 0; i<numDevices; i++){
		deviceInfo = Pa_GetDeviceInfo(i);
		String name(deviceInfo->name);

		__audioInfo.devices.push_back(name);
	}
	__audioInfo.defaultDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(PaHostApiTypeId::paDirectSound))->defaultOutputDevice;
	Preference p("audiodev");
	if (p.isEmpty()) {
		p.setValue(__audioInfo.defaultDevice);
	}



	return initializeSoundStream(p.getValue());


}




static void UpdateCapture(LPARAM lParam)
{
	if ((lParam & MK_LBUTTON) || (lParam & MK_MBUTTON) || (lParam & MK_RBUTTON))
		SetCapture(__hwnd);
	else
		ReleaseCapture();
}


LRESULT CALLBACK __WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//static gameplay::Game* game = gameplay::Game::getInstance();

	if (/*!game->isInitialized() ||*/ hwnd != __hwnd)
	{
		// Ignore messages that are not for our game window.
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	static bool shiftDown = false;
	static bool capsOn = false;

	switch (msg)
	{
	case WM_CLOSE:
#ifdef GP_USE_MEM_LEAK_DETECTION
		DestroyWindow(__hwnd);
#else
		exit(0);
#endif
		return 0;

	case WM_DESTROY:
		Platform::shutdown();
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
	{


						   UpdateCapture(wParam);
						   /*if (!gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_PRESS_LEFT_BUTTON, x, y, 0))
						   {
							   gameplay::Platform::touchEventInternal(gameplay::Touch::TOUCH_PRESS, x, y, 0, true);
						   }*/
						   TouchEvent e;
						   e.id = 0;
						   e.time = Utils::getCurrentMs();
						   e.rawX = GET_X_LPARAM(lParam);
						   e.rawY = GET_Y_LPARAM(lParam);
						   e.type = TouchEvent::DOWN;
						   Application::getInstance()->touchEventInternal(e);

						   return 0;
	}
	case WM_LBUTTONUP:
	{
		TouchEvent e;
		e.id = 0;
		e.time = Utils::getCurrentMs();
		e.rawX = GET_X_LPARAM(lParam);
		e.rawY = GET_Y_LPARAM(lParam);
		e.type = TouchEvent::UP;
		Application::getInstance()->touchEventInternal(e);
						 /*if (!gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_RELEASE_LEFT_BUTTON, x, y, 0))
						 {
							 gameplay::Platform::touchEventInternal(gameplay::Touch::TOUCH_RELEASE, x, y, 0, true);
						 }*/

						 UpdateCapture(wParam);
						 break;
	}
	/*case WM_RBUTTONDOWN:
		UpdateCapture(wParam);
		gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_PRESS_RIGHT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0);
		break;

	case WM_RBUTTONUP:
		gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_RELEASE_RIGHT_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0);
		UpdateCapture(wParam);
		break;

	case WM_MBUTTONDOWN:
		UpdateCapture(wParam);
		gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_PRESS_MIDDLE_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0);
		break;

	case WM_MBUTTONUP:
		gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_RELEASE_MIDDLE_BUTTON, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0);
		UpdateCapture(wParam);
		break;*/

	case WM_MOUSEMOVE:
	{

						 /*if (__mouseCaptured)
						 {
							 // If the incoming position is the mouse capture point, ignore this event
							 // since this is the event that warped the cursor back.
							 if (x == __mouseCapturePoint.x && y == __mouseCapturePoint.y)
								 break;

							 // Convert to deltas
							 x -= __mouseCapturePoint.x;
							 y -= __mouseCapturePoint.y;

							 // Warp mouse back to center of screen.
							 WarpMouse(__mouseCapturePoint.x, __mouseCapturePoint.y);
						 }

						 // Allow Game::mouseEvent a chance to handle (and possibly consume) the event.
						 if (!gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_MOVE, x, y, 0))
						 {*/
							 //if ((wParam & MK_LBUTTON) == MK_LBUTTON)
							 //{
								 // Mouse move events should be interpreted as touch move only if left mouse is held and the game did not consume the mouse event.
								 //gameplay::Platform::touchEventInternal(gameplay::Touch::TOUCH_MOVE, x, y, 0, true);
								 TouchEvent e;
								 e.id = 0;
								 e.time = Utils::getCurrentMs();
								 e.rawX = GET_X_LPARAM(lParam);
								 e.rawY = GET_Y_LPARAM(lParam);
								 e.type = TouchEvent::MOVE;
								 Application::getInstance()->touchEventInternal(e);

							 //}
						 //}
						 break;
	}

	case WM_MOUSEWHEEL:
	{


		TouchEvent e;
		e.id = 0;
		e.time = Utils::getCurrentMs();
		tagPOINT point;
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		ScreenToClient(__hwnd, &point);
		e.rawX = point.x;
		e.rawY = point.y;
		e.type = TouchEvent::WHEEL;
		e.wheel = GET_WHEEL_DELTA_WPARAM(wParam);
		Application::getInstance()->touchEventInternal(e);
		ScreenToClient(__hwnd, &point);
		break;
	}

	/*case WM_MOUSEWHEEL:
		tagPOINT point;
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		ScreenToClient(__hwnd, &point);
		gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_WHEEL, point.x, point.y, GET_WHEEL_DELTA_WPARAM(wParam) / 120);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_SHIFT || wParam == VK_LSHIFT || wParam == VK_RSHIFT)
			shiftDown = true;

		if (wParam == VK_CAPITAL)
			capsOn = !capsOn;

		gameplay::Platform::keyEventInternal(gameplay::Keyboard::KEY_PRESS, getKey(wParam, shiftDown ^ capsOn));
		break;

	case WM_KEYUP:
		if (wParam == VK_SHIFT || wParam == VK_LSHIFT || wParam == VK_RSHIFT)
			shiftDown = false;

		gameplay::Platform::keyEventInternal(gameplay::Keyboard::KEY_RELEASE, getKey(wParam, shiftDown ^ capsOn));
		break;

	case WM_CHAR:
		gameplay::Platform::keyEventInternal(gameplay::Keyboard::KEY_CHAR, wParam);
		break;

	case WM_UNICHAR:
		gameplay::Platform::keyEventInternal(gameplay::Keyboard::KEY_CHAR, wParam);
		break;

	case WM_SETFOCUS:
		break;

	case WM_KILLFOCUS:
		break;*/

	case WM_SIZE:
		// Window was resized.
		Application::getInstance()->resizeEventInternal(LOWORD(lParam), HIWORD(lParam));
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}



bool createWindow(WindowCreationParams* params, HWND* hwnd, HDC* hdc)
{
	bool fullscreen = false;
	bool resizable = false;
	RECT rect = { CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT };

	if (params)
	{
		memcpy(&rect, &params->rect, sizeof(RECT));
		fullscreen = params->fullscreen;
		resizable = params->resizable;
	}

	// Set the window style.
	DWORD style, styleEx;
	if (fullscreen)
	{
		style = WS_POPUP;
		styleEx = WS_EX_APPWINDOW;
	}
	else
	{
		if (resizable)
			style = WS_OVERLAPPEDWINDOW;
		else
			style = WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU;
		styleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	}
	style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	// Adjust the window rectangle so the client size is the requested size.
	AdjustWindowRectEx(&rect, style, FALSE, styleEx);

	// Create the native Windows window.
	*hwnd = CreateWindowEx(styleEx, L"gameplay", L"windownm", style, 0, 0, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, __hinstance, NULL);
	if (*hwnd == NULL)
	{
		LOGE("Failed to create window.");
		return false;
	}

	// Get the drawing context.
	*hdc = GetDC(*hwnd);
	if (*hdc == NULL)
	{
		LOGE("Failed to get device context.");
		return false;
	}

	// Center the window
	GetWindowRect(*hwnd, &rect);
	const int screenX = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
	const int screenY = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
	SetWindowPos(*hwnd, *hwnd, screenX, screenY, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	return true;
}

bool initializeGL(WindowCreationParams* params)
{
	// Create a temporary window and context to we can initialize GLEW and get access
	// to additional OpenGL extension functions. This is a neccessary evil since the
	// function for querying GL extensions is a GL extension itself.
	HWND hwnd = NULL;
	HDC hdc = NULL;
	if (params){
		if (!createWindow(params, &hwnd, &hdc))
			return false;
	}else{
		hwnd = __hwnd;
		hdc = __hdc;
	}



	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = DEFAULT_COLOR_BUFFER_SIZE;
	pfd.cDepthBits = DEFAULT_DEPTH_BUFFER_SIZE;
	pfd.cStencilBits = DEFAULT_STENCIL_BUFFER_SIZE;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (pixelFormat == 0)
	{
		DestroyWindow(hwnd);
		LOGE("Failed to choose a pixel format.");
		return false;
	}

	if (!SetPixelFormat(hdc, pixelFormat, &pfd))
	{
		DestroyWindow(hwnd);
		LOGE("Failed to set the pixel format.");
		return false;
	}

	HGLRC tempContext = wglCreateContext(hdc);
	if (!tempContext)
	{
		DestroyWindow(hwnd);
		LOGE("Failed to create temporary context for initialization.");
		return false;
	}
	wglMakeCurrent(hdc, tempContext);

	// Initialize GLEW
	if (GLEW_OK != glewInit())
	{
		wglDeleteContext(tempContext);
		DestroyWindow(hwnd);
		LOGE("Failed to initialize GLEW.");
		return false;
	}

	if (wglChoosePixelFormatARB && wglCreateContextAttribsARB)
	{
		// Choose pixel format using wglChoosePixelFormatARB, which allows us to specify
		// additional attributes such as multisampling.
		//
		// Note: Keep multisampling attributes at the start of the attribute lists since code below
		// assumes they are array elements 0 through 3.
		int attribList[] = {
			//WGL_SAMPLES_ARB, params->samples ? params->samples : 0,
			//WGL_SAMPLE_BUFFERS_ARB, params ? (params->samples > 0 ? 1 : 0) : 0,
			WGL_SAMPLES_ARB, 0,
			WGL_SAMPLE_BUFFERS_ARB, 0,
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, DEFAULT_COLOR_BUFFER_SIZE,
			WGL_DEPTH_BITS_ARB, DEFAULT_DEPTH_BUFFER_SIZE,
			WGL_STENCIL_BITS_ARB, DEFAULT_STENCIL_BUFFER_SIZE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			0
		};


		__multiSampling = params && params->samples > 0;

		UINT numFormats;
		if (!wglChoosePixelFormatARB(hdc, attribList, NULL, 1, &pixelFormat, &numFormats) || numFormats == 0)
		{
			bool valid = false;
			if (params && params->samples > 0)
			{
				LOGW("Failed to choose pixel format with WGL_SAMPLES_ARB == %d. Attempting to fallback to lower samples setting.", params->samples);
				while (params->samples > 0)
				{
					params->samples /= 2;
					attribList[1] = params->samples;
					attribList[3] = params->samples > 0 ? 1 : 0;
					if (wglChoosePixelFormatARB(hdc, attribList, NULL, 1, &pixelFormat, &numFormats) && numFormats > 0)
					{
						valid = true;
						LOGW("Found pixel format with WGL_SAMPLES_ARB == %d.", params->samples);
						break;
					}
				}

				__multiSampling = params->samples > 0;
			}

			if (!valid)
			{
				wglDeleteContext(tempContext);
				DestroyWindow(hwnd);
				LOGE("Failed to choose a pixel format.");
				return false;
			}
		}

		// Create new/final window if needed
		if (params)
		{
			DestroyWindow(hwnd);
			hwnd = NULL;
			hdc = NULL;

			if (!createWindow(params, &__hwnd, &__hdc))
			{
				wglDeleteContext(tempContext);
				return false;
			}
		}

		// Set final pixel format for window
		if (!SetPixelFormat(__hdc, pixelFormat, &pfd))
		{
			LOGE("Failed to set the pixel format: %d.", (int)GetLastError());
			return false;
		}

		// Create our new GL context
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 0,
			0
		};
		//MessageBoxA(0, (char*)glGetstd::string(GL_SHADING_LANGUAGE_VERSION), "OPENGL VERSION", 0);

		if (!(__hrc = wglCreateContextAttribsARB(__hdc, 0, attribs)))
		{
			wglDeleteContext(tempContext);
			LOGE("Failed to create OpenGL context.");
			return false;
		}

		// Delete the old/temporary context and window
		wglDeleteContext(tempContext);

		// Make the new context current
		if (!wglMakeCurrent(__hdc, __hrc) || !__hrc)
		{
			LOGE("Failed to make the window current.");
			return false;
		}
	}
	else    // fallback to OpenGL 2.0 if wglChoosePixelFormatARB or wglCreateContextAttribsARB is NULL.
	{
		// Context is already here, just use it.
		__hrc = tempContext;
		__hwnd = hwnd;
		__hdc = hdc;
	}

	// Vertical sync.
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(__vsync ? 1 : 0);
	else
		__vsync = false;

	// Some old graphics cards support EXT_framebuffer_object instead of ARB_framebuffer_object.
	// Patch ARB_framebuffer_object functions to EXT_framebuffer_object ones since semantic is same.
	if (!GLEW_ARB_framebuffer_object && GLEW_EXT_framebuffer_object)
	{
		glBindFramebuffer = glBindFramebufferEXT;
		glBindRenderbuffer = glBindRenderbufferEXT;
		glBlitFramebuffer = glBlitFramebufferEXT;
		glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
		glDeleteFramebuffers = glDeleteFramebuffersEXT;
		glDeleteRenderbuffers = glDeleteRenderbuffersEXT;
		glFramebufferRenderbuffer = glFramebufferRenderbufferEXT;
		glFramebufferTexture1D = glFramebufferTexture1DEXT;
		glFramebufferTexture2D = glFramebufferTexture2DEXT;
		glFramebufferTexture3D = glFramebufferTexture3DEXT;
		glFramebufferTextureLayer = glFramebufferTextureLayerEXT;
		glGenFramebuffers = glGenFramebuffersEXT;
		glGenRenderbuffers = glGenRenderbuffersEXT;
		glGenerateMipmap = glGenerateMipmapEXT;
		glGetFramebufferAttachmentParameteriv = glGetFramebufferAttachmentParameterivEXT;
		glGetRenderbufferParameteriv = glGetRenderbufferParameterivEXT;
		glIsFramebuffer = glIsFramebufferEXT;
		glIsRenderbuffer = glIsRenderbufferEXT;
		glRenderbufferStorage = glRenderbufferStorageEXT;
		glRenderbufferStorageMultisample = glRenderbufferStorageMultisampleEXT;
	}

	return true;
}






Platform::Platform() {

}
Platform::~Platform() {
	if (__hwnd)
	{
		DestroyWindow(__hwnd);
		__hwnd = 0;
	}
}

int Platform::start(){

		Application *app = Application::getInstance();
		//ASSERT(mApp);

		SwapBuffers(__hdc);

		if (app->getState() != Application::RUNNING)
			app->run();

		// Enter event dispatch loop.
		MSG msg;
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
				{
					Platform::shutdown();
					return msg.wParam;
				}
			}
			else
			{
				app->frame();
				SwapBuffers(__hdc);
			}

			// If we are done, then exit.
			if (app->getState() == Application::UNINITIALIZED)
				break;
		}
		return 0;
}
void Platform::shutdown(){
	Application::getInstance()->shutdown();
}


bool Platform::create(){
	//ASSERT(mApp);

	//FileSystem::setResourcePath("./");


	// Get the application module handle.
	__hinstance = ::GetModuleHandle(NULL);

	// Read window settings from config.
	WindowCreationParams params;
	params.fullscreen = false;
	params.resizable = true;
	params.rect.left = 0;
	params.rect.top = 0;
	params.rect.right = 0;
	params.rect.bottom = 0;
	params.samples = 4;


	// If window size was not specified, set it to a default value
	if (params.rect.right == 0)
		params.rect.right = params.rect.left + DEFAULT_RESOLUTION_X;
	if (params.rect.bottom == 0)
		params.rect.bottom = params.rect.top + DEFAULT_RESOLUTION_Y;
	int width = params.rect.right - params.rect.left;
	int height = params.rect.bottom - params.rect.top;

	if (params.fullscreen)
	{
		// Enumerate all supposed display settings
		bool modeSupported = false;
		DWORD modeNum = 0;
		DEVMODE devMode;
		memset(&devMode, 0, sizeof(DEVMODE));
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmDriverExtra = 0;
		while (EnumDisplaySettings(NULL, modeNum++, &devMode) != 0)
		{
			// Is mode supported?
			if (devMode.dmPelsWidth == width &&
				devMode.dmPelsHeight == height &&
				devMode.dmBitsPerPel == DEFAULT_COLOR_BUFFER_SIZE)
			{
				modeSupported = true;
				break;
			}
		}

		// If the requested mode is not supported, fall back to a safe default
		if (!modeSupported)
		{
			width = DEFAULT_RESOLUTION_X;
			height = DEFAULT_RESOLUTION_Y;
			params.rect.right = params.rect.left + width;
			params.rect.bottom = params.rect.top + height;
		}
	}


	// Register our window class.
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)__WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = __hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;  // No brush - we are going to paint our own background
	wc.lpszMenuName = NULL;  // No default menu
	wc.lpszClassName = L"gameplay";

	if (!::RegisterClassEx(&wc))
	{
		LOGE("Failed to register window class.");
		goto error;
	}

	if (params.fullscreen)
	{
		DEVMODE dm;
		memset(&dm, 0, sizeof(dm));
		dm.dmSize = sizeof(dm);
		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;
		dm.dmBitsPerPel = DEFAULT_COLOR_BUFFER_SIZE;
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try to set selected mode and get results. NOTE: CDS_FULLSCREEN gets rid of start bar.
		if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			params.fullscreen = false;
			LOGE("Failed to start game in full-screen mode with resolution %dx%d.", width, height);
			goto error;
		}
	}

	if (!initializeGL(&params))
		goto error;

	/*__soundStream = initializeSound();
	if (!__soundStream)
		goto error;*/

	#ifdef USE_MIDI
	if (!initializeMidi())
		goto error;
	#endif

	// Show the window.
	ShowWindow(__hwnd, SW_SHOW);
	return true;


error:

	system("PAUSE");
	exit(0);
	return false;
}
unsigned int Platform::getDisplayWidth(){
	static RECT rect;
	GetClientRect(__hwnd, &rect);
	return rect.right;
}

unsigned int Platform::getDisplayHeight(){
	static RECT rect;
	GetClientRect(__hwnd, &rect);
	return rect.bottom;
}



double Platform::getDisplaySize(){
	HDC hdc = (!__hdc) ? GetWindowDC(__hwnd) : __hdc;

	int hsize = GetDeviceCaps(hdc, HORZSIZE);
	int vsize = GetDeviceCaps(hdc, VERTSIZE);
	
	//int hsize = getDisplayWidth();
	//int vsize = getDisplayHeight();

	double diagonal = sqrt((hsize*hsize) + (vsize*vsize));
	return ((double)diagonal / 10) / 2.54;
}
double Platform::getDisplayDensity(){
	HDC hdc = (!__hdc) ? GetWindowDC(NULL) : __hdc;

	int hres = GetDeviceCaps(hdc, HORZRES);
	int vres = GetDeviceCaps(hdc, VERTRES);
	double diagonalres = sqrt((hres*hres) + (vres*vres));
	return diagonalres / Platform::getDisplaySize();
}
String Platform::getStorageDir(){
	WCHAR p[MAX_PATH];
	GetModuleFileName(NULL, p, MAX_PATH);
	PathRemoveFileSpec(p);
	String s;
	s.fromWide(p);

	return Utils::formatPath(Utils::formatPath(s) + std::string("cache"));
}
#include <filesystem>
String Platform::getTempDir() {
	return String(std::experimental::filesystem::temp_directory_path());
}
#include <Shlobj.h>
String Platform::getSystemMusicDir(){
	wchar_t buffer[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_MYMUSIC, NULL, SHGFP_TYPE_CURRENT, buffer);
	return String(buffer);
}
bool Platform::saveToVideos(String filepath, String newfilename) {
	wchar_t buffer[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_MYVIDEO, NULL, SHGFP_TYPE_CURRENT, buffer);
	auto videopat = String(buffer) + "\\" + newfilename;
	BOOL b = CopyFile(filepath.toWide().c_str(), videopat.toWide().c_str(), 0);
	if (!b) {
		return false;
	}
	else {
		return true;
	}
}


String Platform::getAssetsDir(){
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	PathRemoveFileSpec(buffer);
	return String(buffer) + String("/assets/");
}


String Platform::displayFileDialog(const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
{
	std::string filename;
	OPENFILENAMEA ofn;
	memset(&ofn, 0, sizeof(ofn));

	// Set initial directory
	std::string initialDirectoryStr;
	char currentDir[256];
	if (initialDirectory == NULL)
	{
		char currentDir[512];
		GetCurrentDirectoryA(512, currentDir);
		initialDirectoryStr = currentDir;
	}
	else
	{
		initialDirectoryStr = initialDirectory;
	}

	// Filter on extensions
	std::stringstream f(filterExtensions);
	std::string s;
	unsigned int count = 0;
	std::string descStr = filterDescription;
	descStr += " (";
	std::string extStr = "";
	while (std::getline(f, s, ';'))
	{
		if (count > 0)
			extStr += ";";
		extStr += "*.";
		extStr += s;
		count++;
	}
	descStr += extStr;
	descStr += ")";
	char filter[1024];
	memset(filter, 0, 1024);
	strcpy(filter, descStr.c_str());
	strcpy(filter + descStr.length() + 1, extStr.c_str());

	char szFileName[512] = "";
	ofn.lpstrFile = szFileName;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetForegroundWindow();
	ofn.lpstrTitle = title;
	ofn.lpstrFilter = filter;
	ofn.lpstrInitialDir = initialDirectoryStr.c_str();
	ofn.nMaxFile = 512;
	ofn.lpstrDefExt = filter;


		ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
		GetOpenFileNameA(&ofn);



	filename = szFileName;

	if (initialDirectory == NULL)
		SetCurrentDirectoryA(currentDir);

	return filename;

}


Platform::AudioInfo Platform::getAudioInfo() {
	return __audioInfo;
}
bool Platform::setAudioDevice(int index) {
	if (__soundStream) Pa_CloseStream(__soundStream);
	__soundStream = NULL;
	__soundStream = initializeSoundStream(index);
	if (__soundStream) return true;
	return false;
}

double Platform::getDisplayScale() {
	return 2.0;
}
void Platform::openMediaSelector(int type, int source) {
	wchar_t buffer[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_MYVIDEO, NULL, SHGFP_TYPE_CURRENT, buffer);

	auto res = Platform::displayFileDialog("Select video", "", "", String(buffer).c_str());
	
	Application::getInstance()->getPageController()->getCurrentPage()->onResult(&res);

}
#endif
