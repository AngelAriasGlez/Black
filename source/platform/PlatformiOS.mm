#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE

#import <AudioToolbox/AudioToolbox.h>
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <MobileCoreServices/UTCoreTypes.h>


#include "Platform.hpp"
#include "Application.hpp"
#include "../utils/Log.hpp"

extern const int WINDOW_SCALE = [[UIScreen mainScreen] scale];

CGSize DeviceOrientedSize(){

    CGFloat width   = [[UIScreen mainScreen] bounds].size.width;
    CGFloat height  = [[UIScreen mainScreen] bounds].size.height;
    CGRect bounds = CGRectZero;
    if (UIDeviceOrientationIsLandscape([UIDevice currentDevice].orientation)) {
        bounds.size = CGSizeMake(MAX(width, height)* WINDOW_SCALE, MIN(width, height)* WINDOW_SCALE);
    } else {
        bounds.size = CGSizeMake(MIN(width, height)* WINDOW_SCALE, MAX(width, height)* WINDOW_SCALE);
    }
    return bounds.size;
};
CGPoint DeviceOrientedTouch(CGPoint ipoint){
    //CGSize s = DeviceOrientedSize();
    CGPoint point = {ipoint.x * (CGFloat)WINDOW_SCALE, ipoint.y * (CGFloat)WINDOW_SCALE};
    return point;
};


@interface PickerController : GLKViewController{}
@end
@interface PickerController () <UINavigationControllerDelegate, UIImagePickerControllerDelegate, UIVideoEditorControllerDelegate>
@end

@implementation PickerController

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info{
    NSURL *videoURL = [info objectForKey:UIImagePickerControllerMediaURL];
    auto p = Application::getInstance()->getPageController()->getCurrentPage();
    if(p){
        std::string s([videoURL.path UTF8String]);
        p->onResult(&s);
    }
    //NSLog(@"VideoURL = %@", videoURL);
    [picker dismissViewControllerAnimated:YES completion:NULL];
}
-(void)imagePickerControllerDidCancel:(UIImagePickerController *)picker{
    [picker dismissViewControllerAnimated:YES completion:nil];
}
@end



void checkStatus(OSStatus status){
    
    if (status != noErr){
        NSError *err = [NSError errorWithDomain:NSOSStatusErrorDomain
                                           code:status userInfo:nil];
        NSLog(@"Error: %@", [err description]);
    }
    
}
static OSStatus playbackCallback(void *inRefCon,AudioUnitRenderActionFlags *ioActionFlags,const AudioTimeStamp *inTimeStamp,UInt32 inBusNumber,UInt32 inNumberFrames,
AudioBufferList *ioData) {
    for (int i=0; i < ioData->mNumberBuffers; i++) {
        AudioBuffer buffer = ioData->mBuffers[i];
        UInt32 size = buffer.mDataByteSize;
        Application::getInstance()->soundStreamInternal(buffer.mData, size/8);
        
        // uncomment to hear random noise
         /*float *frameBuffer = (float*)buffer.mData;
         for (int j = 0; j < size/4; j++) {
             auto z = (static_cast <float> (rand()*2.) / static_cast <float> (RAND_MAX))- 1.0f;
             frameBuffer[j] = z;
         }*/
        
    }
    return noErr;
}
mt::Point pinch0;
mt::Point pinch1;

mt::Point pan0;
mt::Point pan1;

mt::Point prevPan;

/****** VIEW CONROLLER****************************************************************************/
@interface ViewController: GLKViewController
@end
@interface ViewController () {}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
- (void)setupAudio;
@end
@implementation ViewController
- (void)viewDidLoad{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    view.drawableMultisample = GLKViewDrawableMultisampleNone;
    
    self.preferredFramesPerSecond = 1000;
    
    
    //HideStatusBar
    if ([self respondsToSelector:@selector(setNeedsStatusBarAppearanceUpdate)]) {
        // iOS 7
        [self prefersStatusBarHidden];
        [self performSelector:@selector(setNeedsStatusBarAppearanceUpdate)];
    } else {
        // iOS 6
        [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationSlide];
    }

    @autoreleasepool {
        UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinch:)];
        UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
        panGesture.minimumNumberOfTouches = 2;
        panGesture.maximumNumberOfTouches = 2;
        [self.view addGestureRecognizer:pinchGesture];
        [self.view addGestureRecognizer:panGesture];
    }

    
    [self setupGL];
    [self setupAudio];

}
-(void)handlePan:(UIPanGestureRecognizer*)sender {
    long num = [sender numberOfTouches];
    if(num <= 1) return;
    
    auto p0 = DeviceOrientedTouch([sender locationOfTouch:0 inView:self.view]);
    auto p1 = DeviceOrientedTouch([sender locationOfTouch:1 inView:self.view]);
    auto trans = DeviceOrientedTouch([sender translationInView:self.view]);
    
    if(sender.state == UIGestureRecognizerStateBegan){
        pan0 = mt::Point(p0.x, p0.y);
        pan1 = mt::Point(p1.x, p1.y);
        prevPan = mt::Point(trans.x, trans.y);
    }
    //if(sender.state == UIGestureRecognizerStateChanged){}
    
    auto diff = mt::Point(trans.x-prevPan.x, trans.y-prevPan.y);
    prevPan = mt::Point(trans.x, trans.y);

    
    TouchEvent e;
    e.id = 1;
    e.time = Utils::getCurrentMs();
    e.rawX = p0.x;
    e.rawY = p0.y;
    e.rawX1 = p1.x;
    e.rawY1 = p1.y;
    
    e.gestureDown1 = pan0;
    e.gestureDown2 = pan1;
    
    e.type = TouchEvent::PAN;
    e.dragDiff = diff;
    Application::getInstance()->touchEventInternal(e);
    
}
-(void)handlePinch:(UIPinchGestureRecognizer*)sender {
    
    long num = [sender numberOfTouches];
    if(num <= 1) return;
    
    auto p0 = DeviceOrientedTouch([sender locationOfTouch:0 inView:self.view]);
    auto p1 = DeviceOrientedTouch([sender locationOfTouch:1 inView:self.view]);
    
    if(sender.state == UIGestureRecognizerStateBegan){
        pinch0 = mt::Point(p0.x, p0.y);
        pinch1 = mt::Point(p1.x, p1.y);
    }
    //if(sender.state == UIGestureRecognizerStateChanged){}
    
    
    TouchEvent e;
    e.id = 1;
    e.time = Utils::getCurrentMs();
    e.rawX = p0.x;
    e.rawY = p0.y;
    e.rawX1 = p1.x;
    e.rawY1 = p1.y;
    
    e.gestureDown1 = pinch0;
    e.gestureDown2 = pinch1;
    
 
    e.data = [sender scale];
    
    
    
	
	if(sender.state == UIGestureRecognizerStateBegan){
        e.type = TouchEvent::PINCH_START;
        Application::getInstance()->touchEventInternal(e);
    }else{
        e.type = TouchEvent::PINCH;
        Application::getInstance()->touchEventInternal(e);
    }
    
    //NSLog(@"%f", [sender scale]);
}

- (BOOL)prefersStatusBarHidden{
    return YES;
}
- (void)dealloc{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
    
    // Dispose of any resources that can be recreated.
}

- (void)setupGL{
    [EAGLContext setCurrentContext:self.context];

    Application *app = Application::getInstance();
    if (app->getState() != Application::RUNNING)
        app->run();

}

- (void)tearDownGL{
    [EAGLContext setCurrentContext:self.context];
}

- (void)update{}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect{
    Application *app = Application::getInstance();
    app->frame();
    
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
    for(UITouch* touch in touches)
    {
        CGPoint touchPoint = [touch locationInView:nil];
        touchPoint = DeviceOrientedTouch(touchPoint);

        TouchEvent e;
        e.id = [touch hash];
        e.time = Utils::getCurrentMs();
        e.rawX = touchPoint.x;
        e.rawY = touchPoint.y;
        e.type = TouchEvent::DOWN;
        Application::getInstance()->touchEventInternal(e);
        
    }
    
}
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event{
    for(UITouch* touch in touches)
    {
        CGPoint touchPoint = [touch locationInView:nil];
        touchPoint = DeviceOrientedTouch(touchPoint);
        
        TouchEvent e;
        e.id = [touch hash];
        e.time = Utils::getCurrentMs();
        e.rawX = touchPoint.x;
        e.rawY = touchPoint.y;
        e.type = TouchEvent::UP;
        Application::getInstance()->touchEventInternal(e);
    }
    
}
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{
    for(UITouch* touch in touches)
    {
        CGPoint touchPoint = [touch locationInView:nil];
        touchPoint = DeviceOrientedTouch(touchPoint);

        TouchEvent e;
        e.id = [touch hash];
        e.time = Utils::getCurrentMs();
        e.rawX = touchPoint.x;
        e.rawY = touchPoint.y;
        e.type = TouchEvent::MOVE;
        Application::getInstance()->touchEventInternal(e);
        
    }
    
}

- (void)setupAudio{
    OSStatus status;
    AudioSessionInitialize(NULL, NULL, NULL, (__bridge void*)self);
    UInt32 sessionCategory = kAudioSessionCategory_LiveAudio;
    status = AudioSessionSetProperty (kAudioSessionProperty_AudioCategory, sizeof (sessionCategory), &sessionCategory);
    checkStatus(status);
    float aBufferLength = 0.1; // 0.001 == 64 frames
    status = AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration,sizeof(aBufferLength), &aBufferLength);    // Describe format
    checkStatus(status);
    AudioSessionSetActive(TRUE);
    UInt32 flag = 1;
    AudioComponentInstance audioUnit;
    // Describe audio component
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    // Get component
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
    // Get audio units
    status = AudioComponentInstanceNew(inputComponent, &audioUnit);
    checkStatus(status);
    status = AudioUnitSetProperty(audioUnit,kAudioOutputUnitProperty_EnableIO,kAudioUnitScope_Output,0,&flag,sizeof(flag));
    checkStatus(status);
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate         = 44100.00;
    audioFormat.mFormatID           = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags        = kAudioFormatFlagIsFloat;
    audioFormat.mFramesPerPacket    = 1;
    audioFormat.mChannelsPerFrame   = 2;
    audioFormat.mBitsPerChannel     = 32;
    audioFormat.mBytesPerPacket     = 8;
    audioFormat.mBytesPerFrame      = 8;
    // Apply format
    checkStatus(status);
    status = AudioUnitSetProperty(audioUnit,kAudioUnitProperty_StreamFormat,kAudioUnitScope_Input,0,&audioFormat,sizeof(audioFormat));
    checkStatus(status);
    // Set output callback
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = playbackCallback;
    //callbackStruct.inputProcRefCon = (__bridge void *)self;
    status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callbackStruct, sizeof(callbackStruct));
    checkStatus(status);
    // Initialise
    status = AudioUnitInitialize(audioUnit);
    checkStatus(status);
    status = AudioOutputUnitStart(audioUnit);
    checkStatus(status);
}

@end



@class AppDelegate;
ViewController *__viewController;
AppDelegate *__appDelegate;
PickerController * __pickerController;

/******APP DELEGATE****************************************************************************/

@interface AppDelegate : UIResponder <UIApplicationDelegate>{
    ViewController* viewController;
}
@property (strong, nonatomic) UIWindow *window;
@property (nonatomic, retain) ViewController *viewController;
@end
@implementation AppDelegate
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions{
    __appDelegate = self;
    __pickerController = [[PickerController alloc] init];
    
    self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];

	[nc addObserver:self selector:@selector(orientationChanged:) name:UIDeviceOrientationDidChangeNotification object:[UIDevice currentDevice]];

    viewController = [[ViewController alloc] init];
    [self.window setRootViewController:viewController];
    [self.window makeKeyAndVisible];
    
    return YES;
}
- (void)orientationChanged:(NSNotification *)note{
    NSLog(@"Orientation  has changed: w:%d h:%d", (int)DeviceOrientedSize().width, (int)DeviceOrientedSize().height);
    Application::getInstance()->resizeEventInternal(Platform::getDisplayWidth(), Platform::getDisplayHeight());
}

- (void)applicationWillResignActive:(UIApplication *)application{}
- (void)applicationDidEnterBackground:(UIApplication *)application{}
- (void)applicationWillEnterForeground:(UIApplication *)application{}
- (void)applicationDidBecomeActive:(UIApplication *)application{}
- (void)applicationWillTerminate:(UIApplication *)application{}

- (void)dealloc{
    [self.window setRootViewController:nil];

}
@end





/******PLATFORM METHODS****************************************************************************/



Platform::Platform() {
    
}
Platform::~Platform() {

}

int Platform::start(){
    @autoreleasepool {
        UIApplicationMain(0 ,0 ,nil , NSStringFromClass([AppDelegate class]));
    }
    return EXIT_SUCCESS;

}
void Platform::shutdown(){

}


bool Platform::create(){
    return true;
}


unsigned int Platform::getDisplayWidth()
{
    CGSize size = DeviceOrientedSize();
    return size.width;

}

unsigned int Platform::getDisplayHeight()
{
    CGSize size = DeviceOrientedSize();
    return size.height;
}


double Platform::getDisplaySize(){
    float scale = [[UIScreen mainScreen] scale];
    float ppi = scale * ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) ? 132 : 163);
    float width = ([[UIScreen mainScreen] bounds].size.width * scale);
    float height = ([[UIScreen mainScreen] bounds].size.height * scale);
    
    float horizontal = width / ppi, vertical = height / ppi;
    
    float diagonal = sqrt(pow(horizontal, 2) + pow(vertical, 2));
	return diagonal;
}
double Platform::getDisplayDensity(){
	float scale = [[UIScreen mainScreen] scale];
    
    float ppi = scale * ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) ? 132 : 163);
    
    return ppi;
}

String Platform::getStorageDir(){
    return getAssetsDir();
}
String Platform::getAssetsDir(){
    NSBundle *myBundle = [NSBundle mainBundle];
    return [[NSString stringWithFormat:@"%@/", myBundle.bundlePath] UTF8String];
}

String Platform::displayFileDialog(const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
{
	return getAssetsDir() + std::string("track.mp3");
}
double Platform::getDisplayScale(){
    return [[UIScreen mainScreen] scale];
}

void Platform::openMediaSelector(int type, int source){

    UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
    ipc.delegate = __pickerController ;
    ipc.videoQuality = UIImagePickerControllerQualityTypeHigh;

    if(source == 1){
        ipc.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    }else{
        ipc.sourceType = UIImagePickerControllerSourceTypeCamera;
    }
        
    if(type == 1){
        ipc.mediaTypes = @[(NSString*)kUTTypeVideo, (NSString*)kUTTypeMovie];
    }else{
        ipc.mediaTypes = @[(NSString*)kUTTypeImage];
    }
        
    //[ipc setVideoMaximumDuration:5];
    [[UIApplication sharedApplication].keyWindow.rootViewController presentViewController:ipc animated:YES completion:NULL];
    
}
String Platform::getTempDir(){
    NSURL *tmpDirURL = [NSURL fileURLWithPath:NSTemporaryDirectory() isDirectory:YES];
    return String([tmpDirURL.path UTF8String]) + "/";
}

bool Platform::saveToVideos(String filename, String newfilename){
    NSString *fp = [NSString stringWithCString:filename.c_str() encoding:[NSString defaultCStringEncoding]];
    if ( [[NSFileManager defaultManager] fileExistsAtPath:fp] == NO)
    {
        LOGE("Video to copy not exist!!!: %s", filename.c_str());
        return false;
    }

    NSURL *videoURL = [[NSURL alloc]  initFileURLWithPath:fp isDirectory:NO];
    
    /*
    NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:fp];
    
    [[PHPhotoLibrary sharedPhotoLibrary] performChanges:^
     {
         [PHAssetChangeRequest creationRequestForAssetFromVideoAtFileURL:fileURL];
     }
     completionHandler:^(BOOL success, NSError *error)
     {
         if (success) {
             NSLog(@"Movie saved to camera roll.");
         }
         else {
             NSLog(@"Could not save movie to camera roll. Error: %@", error);
         }
     }];*/
    
    
    
    if (UIVideoAtPathIsCompatibleWithSavedPhotosAlbum(fp)) {
        UISaveVideoAtPathToSavedPhotosAlbum(fp, nil, nil, nil);
        return true;
    }
    return false;
}

#endif
#endif
