#ifndef APPLICATION_H
#define APPLICATION_H

#include <time.h> 
#include <vector>


#include "../ui/PageController.hpp"
#include "../core/GestureDetector.hpp"
class Canvas;

class Application: IGestureListener{
	friend class Platform;
public:
	enum State{
		UNINITIALIZED,
		RUNNING,
		PAUSED
	};

	Application();
	~Application();

	static Application *getInstance();

	int run();
	bool startup();
	void frame();

	State  getState();

	void shutdown();


	
	
	void resizeEventInternal(unsigned int width, unsigned int height);
	void touchEventInternal(TouchEvent e);
	void onGesture(TouchEvent e);
	void soundStreamInternal(void* outputBuffer, int frames);
	void midiStreamInternal(double deltatime, std::vector< unsigned char > *message);

	PageController* getPageController();

    Canvas* getCanvas(){return mCanvas;}


	NVGcontext* getNVGContext() {
		return mNVGContext;
	};
private:
	static Application *mInstance;

	bool mInitialized;

	State mState;

	long mFpsFrameCount;
	long mFpsLastFrame;

	Canvas* mCanvas;
	PageController* mPageController;

	GestureDetector *mGestureDetector;

	NVGcontext *mNVGContext;

protected:
	long mFps;

	int mWidth;
	int mHeight;

    virtual void initializeEvent(){};
    virtual void finalizeEvent(){};
    virtual void renderEvent(Canvas *canvas){};
    
    virtual void resizeEvent(int width, int height){};
	virtual void touchEvent(TouchEvent evn){};
	virtual void soundStreamEvent(void* outputBuffer, int frames){};
	virtual void midiStreamEvent(double deltatime, std::vector< unsigned char > *message){};
};

#endif
