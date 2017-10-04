#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_OSX

#include "Application.hpp"
#include "Platform.hpp"
#include "../utils/Log.hpp"
#include "../preferences/Preference.hpp"
#include "../graphics/Canvas.hpp"
#include "../graphics/Color.hpp"

#import <Cocoa/Cocoa.h>

@class OView;


#include <portaudio.h>
#include <pa_mac_core.h>
PaStream* __soundStream;

bool __fullscreen = false;
bool __multiSampling;
bool __vsync = true;
int __width = 1450;
int __height = 800;

OView* __view;

static Platform::AudioInfo __audioInfo;


#define SAMPLE_INT16 33
#define SAMPLE_FLOAT32 44

#define OUTPUT_FORMAT SAMPLE_FLOAT32


int soundStreamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData){
    Application::getInstance()->soundStreamInternal(outputBuffer, framesPerBuffer);
    return 0;
}



@interface OView : NSOpenGLView <NSWindowDelegate>
{
@public
    CVDisplayLinkRef displayLink;
    NSRecursiveLock* gameLock;
    
@protected

}
@end


@implementation OView

-(void)windowWillClose:(NSNotification*)note
{
    [[NSApplication sharedApplication] terminate:self];
}

-(void) update
{
    [[self openGLContext] makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    
    Application::getInstance()->frame();
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    
}


- (id) initWithFrame: (NSRect) frame
{

    int samples = 4;
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFAMultisample,
        NSOpenGLPFASampleBuffers, static_cast<NSOpenGLPixelFormatAttribute>(samples ? 1 : 0),
        NSOpenGLPFASamples, static_cast<NSOpenGLPixelFormatAttribute>(samples),
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
        0
    };
    
    NSOpenGLPixelFormat* pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    
    if ((self = [super initWithFrame:frame pixelFormat:pf]))
    {
        [self setWantsBestResolutionOpenGLSurface:YES];
        
        /*NSRect backingBounds = [self convertRectToBacking:[self bounds]];
        GLsizei backingPixelWidth  = (GLsizei)(backingBounds.size.width),
        backingPixelHeight = (GLsizei)(backingBounds.size.height);
        
        __width = backingPixelWidth;
        __height =  backingPixelHeight;*/

    }
    
    return self;
}

- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
    @autoreleasepool {
        [self update];
    }
    return kCVReturnSuccess;
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result = [(__bridge OView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (void) prepareOpenGL
{
    [super prepareOpenGL];
    
    Application *app = Application::getInstance();
    if (app->getState() != Application::RUNNING)
        app->run();
    
    
    [[self window] setLevel: NSNormalWindowLevel];
    [[self window] makeKeyAndOrderFront: self];
    [[self window] orderFrontRegardless];
    [[self window] setTitle: @"Framework"];
    
    
    [[self openGLContext] makeCurrentContext];
    GLint swapInt = __vsync ? 1 : 0;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, (__bridge void *)self);
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
    
}


- (void)reshape
{
    
    NSSize size = [ [ _window contentView ] frame ].size;
    __width = size.width*2;
    __height = size.height*2;
    
    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    GLint dim[2] = {__width, __height};
    CGLSetParameter(cglContext, kCGLCPSurfaceBackingSize, dim);
    CGLEnable(cglContext, kCGLCESurfaceBackingSize);
    
    
    Application::getInstance()->resizeEventInternal(__width, __height);

}


- (void) dealloc
{
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
}

- (void) mouseDown: (NSEvent*) event
{
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    TouchEvent e;
    e.id = 0;
    e.time = Utils::getCurrentMs();
    e.rawX = point.x*2;
    e.rawY = ((__height/2) - point.y)*2;
    e.type = TouchEvent::DOWN;
    Application::getInstance()->touchEventInternal(e);
    //LOGI("down %d %d", e.rawX, e.rawY);
}

- (void) mouseUp: (NSEvent*) event
{
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    TouchEvent e;
    e.id = 0;
    e.time = Utils::getCurrentMs();
    e.rawX = point.x*2;
    e.rawY = ((__height/2) - point.y)*2;
    e.type = TouchEvent::UP;
    Application::getInstance()->touchEventInternal(e);
    //LOGI("up %d %d", e.rawX, e.rawY);
    
}
- (void)mouseDragged:(NSEvent *)event {
    
    NSPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    TouchEvent e;
    e.id = 0;
    e.time = Utils::getCurrentMs();
    e.rawX = point.x*2;
    e.rawY = ((__height/2) - point.y)*2;
    e.type = TouchEvent::MOVE;
    Application::getInstance()->touchEventInternal(e);
    //LOGI("move %d %d", e.rawX, e.rawY);
}


@end










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
    __audioInfo.defaultDevice = Pa_GetDefaultOutputDevice();
    Preference p("audio_device");
    if (p.isEmpty() || !Pa_GetDeviceInfo(p.getValue())) {
        p.setValue(__audioInfo.defaultDevice);
    }
    
    
    
    return initializeSoundStream(p.getValue());
    
    
}




Platform::Platform() {
    
}
Platform::~Platform() {

}

int Platform::start(){
    
    __soundStream = initializeSound();
    
    //NSAutoreleasePool* pool = [NSAutoreleasePool new];
    NSApplication* app = [NSApplication sharedApplication];
    NSRect screenBounds = [[NSScreen mainScreen] frame];
    NSRect viewBounds = NSMakeRect(0, 0, __width/2, __height/2);
    
    __view = [[OView alloc] initWithFrame:viewBounds];
    
    if (__view == NULL)
    {
        LOGE("Failed to create view: exiting.");
        return EXIT_FAILURE;
    }
    
    NSRect centered = NSMakeRect(NSMidX(screenBounds) - NSMidX(viewBounds),
                                 NSMidY(screenBounds) - NSMidY(viewBounds),
                                 viewBounds.size.width,
                                 viewBounds.size.height);
    
    
    
    NSWindow* window = [[NSWindow alloc]
                  initWithContentRect:centered
                  styleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                  backing:NSBackingStoreBuffered
                  defer:NO
                  ];

    
    [window setAcceptsMouseMovedEvents:YES];
    [window setContentView:__view];
    [window setDelegate:__view];
    
    [NSApp activateIgnoringOtherApps:YES];

    
    [app run];
    
    //[pool release];
    return EXIT_SUCCESS;

}
void Platform::shutdown(){

}


bool Platform::create(){
    return true;
}


unsigned int Platform::getDisplayWidth()
{
    double sf = [[NSScreen mainScreen] backingScaleFactor];
    return __width;

}

unsigned int Platform::getDisplayHeight()
{
    double sf = [[NSScreen mainScreen] backingScaleFactor];
    return __height;
}


double Platform::getDisplaySize(){

    NSScreen *screen = [NSScreen mainScreen];
    NSDictionary *description = [screen deviceDescription];
    CGSize displayPhysicalSize = CGDisplayScreenSize([[description objectForKey:@"NSScreenNumber"] unsignedIntValue]);

    double size = sqrt(displayPhysicalSize.width*displayPhysicalSize.width+displayPhysicalSize.height*displayPhysicalSize.height) / 2.54 / 10;
	return size;
}
double Platform::getDisplayDensity(){

    NSScreen *screen = [NSScreen mainScreen];
    NSDictionary *description = [screen deviceDescription];
    CGSize displayPhysicalSize = CGDisplayScreenSize([[description objectForKey:@"NSScreenNumber"] unsignedIntValue]);
    NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];

    double dpi = (displayPixelSize.width / displayPhysicalSize.width) * 25.4f;
    return dpi;
}
std::string Platform::getCacheDir(){

    
	return getAssetsDir();
}
std::string Platform::getAssetsDir(){
    NSString* path = [[NSBundle mainBundle] bundlePath];
    
    return std::string([path UTF8String])+"/../assets/";

}

std::string Platform::getMusicDirectoryPath(){
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSMusicDirectory, NSUserDomainMask, YES);
    NSString *musicDirectory = [paths objectAtIndex:0];
    return std::string([musicDirectory UTF8String]);
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


std::string Platform::displayFileDialog(const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
{

    
	return getAssetsDir() + std::string("track.mp3");
}


#endif
#endif
