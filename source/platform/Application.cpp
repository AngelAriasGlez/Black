#include "Application.hpp"

#include "Platform.hpp"
#include "../graphics/Metrics.hpp"

#include "../audio/Player.hpp"
#include "../graphics/Canvas.hpp"
#include "../ui/pwidget/TextView.hpp"

#include "../ui/Page.hpp"
#include "../ui/PageController.hpp"
#include "../utils/Assets.hpp"
#include "../core/Timer.hpp"
#include "../core/String.hpp"


#ifdef TARGET_OS_IPHONE
#define NANOVG_GLES3_IMPLEMENTATION
#else
#define NANOVG_GL3_IMPLEMENTATION
#endif

#include "../nanovg/nanovg_gl.h"
#include "../nanovg/nanovg_gl_utils.h"

#include "../fonts/Fonts.hpp"

#include "../thread/Task.hpp"

Application* Application::mInstance = NULL;

Application::Application() : 
	mInitialized(false),
	mState(UNINITIALIZED),
	mWidth(0),
	mHeight(0),
	mFps(0),
	mFpsFrameCount(0),
	mFpsLastFrame(0)
{
	mInstance = this;

	mPageController = new PageController();
	mGestureDetector = new GestureDetector();
	mGestureDetector->setListener(this);
}
Application::~Application(){
	delete mCanvas;
	delete mPageController;
	delete mGestureDetector;

#ifdef NANOVG_GLES3_IMPLEMENTATION
    nvgDeleteGLES3(mNVGContext);
#else
	nvgDeleteGL3(mNVGContext);
#endif
}
Application* Application::getInstance(){
    if(!mInstance) mInstance = new Application();
    return mInstance;
};
int Application::run(){
	if (mState != UNINITIALIZED)
		return -1;

	mWidth = Platform::getDisplayWidth();
	mHeight = Platform::getDisplayHeight();

	// Start up game systems.
	if (!startup())
	{
		shutdown();
		return -2;
	}

	return 0;
}
PageController* Application::getPageController() {
	return mPageController;
}

bool Application::startup()
{
	if (mState != UNINITIALIZED)
		return false;

#ifdef NANOVG_GLES3_IMPLEMENTATION
    mNVGContext = nvgCreateGLES3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
#else
	mNVGContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
#endif
	nvgCreateFontMem(mNVGContext, "sans-bold", roboto_bold_ttf, roboto_bold_ttf_size, 0);
	nvgCreateFontMem(mNVGContext, "sans", roboto_regular_ttf, roboto_regular_ttf_size, 0);
	nvgCreateFontMem(mNVGContext, "sans-light", roboto_light_ttf, roboto_light_ttf_size, 0);
	nvgCreateFontMem(mNVGContext, "icon", entypo_ttf, entypo_ttf_size, 0);
	nvgCreateFontMem(mNVGContext, "c1", custom_1, custom_1_size, 0);
	mCanvas = new Canvas(mNVGContext);

	mState = RUNNING;

	return true;
}
void Application::shutdown(){
	// Call user finalization.
	if (mState != UNINITIALIZED)
	{

		//Platform::signalShutdown();

		// Call user finalize
		finalizeEvent();


		mState = UNINITIALIZED;
	}

}
//static double deg = 0;
static int mx, my;

void Application::frame(){
	Metrics::getInstance()->setDensity(Platform::getDisplayDensity());
	Metrics::getInstance()->setSize(Platform::getDisplaySize());

	if (!mInitialized){

		initializeEvent();

		mInitialized = true;
		resizeEventInternal(mWidth, mHeight);
	}



	long now = clock();
	long delay = now - mFpsLastFrame;

	if (delay > CLOCKS_PER_SEC) {
		mFpsLastFrame = now;
		mFps = mFpsFrameCount;
		mFpsFrameCount = 0;
	}
	mFpsFrameCount++;

	//mCanvas->clear();
	mCanvas->begin(mWidth, mHeight);



	Page* cpage = mPageController->getCurrentPage();
	if (cpage) {
		//cpage->computeChildsDepth();
		cpage->update(mCanvas);
		cpage->draw(mCanvas);
	}

	renderEvent(mCanvas);


	for (auto t : *Timer::TIMERS) {
		t->update();
	}

	Task::updateAll();

    
	mCanvas->drawFont(Point(), mFps, Font(), Color::GREEN);
	mCanvas->end();
}


Application::State Application::getState(){
	return mState;
}

void Application::resizeEventInternal(unsigned int width, unsigned int height){

	if (mWidth != width || mHeight != height){
		mWidth = width;
		mHeight = height;
        //mCanvas->setMainViewport(0, 0, mWidth, mHeight);
        mPageController->resizeEvent();

		resizeEvent(width, height);
        //LOGI("resize %d %d", mWidth, mHeight);
	}
}
void Application::touchEventInternal(TouchEvent evn){
	mGestureDetector->processEvent(evn);
}
void Application::onGesture(TouchEvent evn) {

	Page* cpage = mPageController->getCurrentPage();
	if (cpage) {
		cpage->dispatchTouchEvent(evn);
	}
	mx = evn.rawX;
	my = evn.rawY;
	touchEvent(evn);
};

void Application::soundStreamInternal(void* outputBuffer, int frames){
	Player::getInstance()->read((Sample *)outputBuffer, frames);
	soundStreamEvent(outputBuffer, frames);

}
void Application::midiStreamInternal(double deltatime, std::vector< unsigned char > *message){
	midiStreamEvent(deltatime, message);

}
