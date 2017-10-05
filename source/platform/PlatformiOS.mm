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

UIDeviceOrientation __orientation;

CGSize DeviceOrientedSize(){
    
    if(UIDeviceOrientationIsPortrait(__orientation)){
        return CGSizeMake([[UIScreen mainScreen] bounds].size.width * WINDOW_SCALE, [[UIScreen mainScreen] bounds].size.height * WINDOW_SCALE);
    }else{
        return CGSizeMake([[UIScreen mainScreen] bounds].size.height * WINDOW_SCALE, [[UIScreen mainScreen] bounds].size.width * WINDOW_SCALE);
    }
};
CGPoint DeviceOrientedTouch(CGPoint ipoint){
    CGSize s = DeviceOrientedSize();
    
    CGPoint point = {ipoint.x * (CGFloat)WINDOW_SCALE, ipoint.y * (CGFloat)WINDOW_SCALE};

    
    if(__orientation == UIDeviceOrientationPortrait){

    }else if(__orientation == UIDeviceOrientationPortraitUpsideDown){
        //point.x = (ipoint.x * (CGFloat)WINDOW_SCALE) - s.width;
        //point.y = (ipoint.y * (CGFloat)WINDOW_SCALE) - s.height;
    }else if(__orientation == UIDeviceOrientationLandscapeLeft){
        point.x = ipoint.y * (CGFloat)WINDOW_SCALE;
        point.y = s.height - (ipoint.x * (CGFloat)WINDOW_SCALE);
    }else if(__orientation == UIDeviceOrientationLandscapeRight){
        point.x = s.width - (ipoint.y * (CGFloat)WINDOW_SCALE);
        point.y = ipoint.x * (CGFloat)WINDOW_SCALE;
    }
    return point;
};


@interface PickerController : GLKViewController{}
@end
@interface PickerController () <UINavigationControllerDelegate, UIImagePickerControllerDelegate, UIVideoEditorControllerDelegate>
@end

@implementation PickerController
- (id) init {
    self = [super init];
    if (self != nil) {
        // initializations go here.
    }
    return self;
}
/*- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
    
    // This is the NSURL of the video object
    NSURL *videoURL = [info objectForKey:UIImagePickerControllerMediaURL];
    
    NSLog(@"VideoURL = %@", videoURL);
    [picker dismissViewControllerAnimated:YES completion:NULL];
}*/

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    NSURL *videoURL = [info objectForKey:UIImagePickerControllerMediaURL];
    
    /*UIVideoEditorController* videoEditor = [[UIVideoEditorController alloc] init];
    videoEditor.delegate=self;
    
    if ( [UIVideoEditorController canEditVideoAtPath:videoURL.absoluteString] )
    {
        videoEditor.videoPath = videoURL.absoluteString;
        videoEditor.videoMaximumDuration = 10.0;
        
        //[self.customAvPlayerView addSubview:videoEditor.view];
        
        [picker presentViewController:videoEditor animated:YES completion:nil];
    }
    else
    {
        NSLog( @"can't edit video at %@", videoURL );
    }*/
    
    NSLog(@"VideoURL = %@", videoURL);
    //[picker dismissViewControllerAnimated:YES completion:NULL];
}

-(void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissViewControllerAnimated:YES completion:nil];
}

@end



@interface ViewController: GLKViewController

@end

void checkStatus(OSStatus status){
    
    if (status != noErr){
        NSError *err = [NSError errorWithDomain:NSOSStatusErrorDomain
                                           code:status userInfo:nil];
        NSLog(@"Error: %@", [err description]);
    }
    
}

static OSStatus playbackCallback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
      
                                 
                                 AudioBufferList *ioData) {
    
    
    for (int i=0; i < ioData->mNumberBuffers; i++) {
        AudioBuffer buffer = ioData->mBuffers[i];
        UInt32 size = buffer.mDataByteSize;

        Application::getInstance()->soundStreamInternal(buffer.mData, size/8);
        
        // uncomment to hear random noise
        /*
         UInt16 *frameBuffer = buffer.mData;
         for (int j = 0; j < inNumberFrames; j++) {
         frameBuffer[j] = rand();
         }
        */
        
    }


    
    /*for(int i=0;i<inNumberFrames;i++){
		((short *)ioData->mBuffers[0].mData)[i] = (short)(audiobuffer[i] * SHRT_MAX);

	}*/
    

    return noErr;
}



@interface ViewController () {
    
    
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
- (void)setupAudio;

@end

@implementation ViewController

- (void)viewDidLoad
{
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
    
    self.preferredFramesPerSecond = 30;
    
    
    //HideStatusBar
    if ([self respondsToSelector:@selector(setNeedsStatusBarAppearanceUpdate)]) {
        // iOS 7
        [self prefersStatusBarHidden];
        [self performSelector:@selector(setNeedsStatusBarAppearanceUpdate)];
    } else {
        // iOS 6
        [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationSlide];
    }
    
    
    
    [self setupGL];

}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}
- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
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

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    
    Application *app = Application::getInstance();
    
    
    if (app->getState() != Application::RUNNING)
        app->run();
    
    [self setupAudio];
    
    
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{

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
    
    // ...
    
    
    OSStatus status;
    
    AudioSessionInitialize(NULL, NULL, NULL, (__bridge void*)self);
    UInt32 sessionCategory = kAudioSessionCategory_LiveAudio;
    status = AudioSessionSetProperty (kAudioSessionProperty_AudioCategory, sizeof (sessionCategory), &sessionCategory);
    checkStatus(status);
    
    float aBufferLength = 0.1; // 0.001 == 64 frames
    status = AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration,
                            sizeof(aBufferLength), &aBufferLength);    // Describe format
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
    
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Output,
                                  0,
                                  &flag,
                                  sizeof(flag));
    checkStatus(status);

    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate                 = 44100.00;
    audioFormat.mFormatID                   = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags                = kAudioFormatFlagIsFloat;
    audioFormat.mFramesPerPacket    = 1;
    audioFormat.mChannelsPerFrame   = 2;
    audioFormat.mBitsPerChannel             = 32;
    audioFormat.mBytesPerPacket             = 8;
    audioFormat.mBytesPerFrame              = 8;
    
    // Apply format
    checkStatus(status);
    status = AudioUnitSetProperty(audioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  0,
                                  &audioFormat,
                                  sizeof(audioFormat));
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


@interface AppDelegate : UIResponder <UIApplicationDelegate>{
    ViewController* viewController;

}

@property (strong, nonatomic) UIWindow *window;
@property (nonatomic, retain) ViewController *viewController;
@end

ViewController *__viewController;
AppDelegate *__appDelegate;

PickerController * __pickerController;

@implementation AppDelegate

@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
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

//********** ORIENTATION CHANGED **********
- (void)orientationChanged:(NSNotification *)note{
    UIDeviceOrientation o = [[note object] orientation];
    if(UIDeviceOrientationIsPortrait(o) || UIDeviceOrientationIsLandscape(o)){
    
    __orientation = [[note object] orientation];
    
        NSLog(@"Orientation  has changed: %d", [[note object] orientation]);
    
    Application::getInstance()->resizeEventInternal(Platform::getDisplayWidth(), Platform::getDisplayHeight());
    }
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

- (void)dealloc
{
    [self.window setRootViewController:nil];

}


@end









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

String Platform::getCacheDir(){

    
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


void imagePickerController(UIImagePickerController *picker, NSDictionary *info) {
    
    // This is the NSURL of the video object
    NSURL *videoURL = [info objectForKey:UIImagePickerControllerMediaURL];
    
    NSLog(@"VideoURL = %@", videoURL);
    [picker dismissViewControllerAnimated:YES completion:NULL];
}


void Platform::openMediaSelector(){
    
    /*UIImagePickerController *imagePicker = [[UIImagePickerController alloc] init];
    imagePicker.delegate = __pickerController;
    imagePicker.sourceType = UIImagePickerControllerSourceTypeCamera;
    //imagePicker.mediaTypes = [[NSArray alloc] initWithObjects:(NSString *)kUTTypeMovie,      nil];
    //imagePicker.mediaTypes = @[(NSString*)kUTTypeMovie, (NSString*)kUTTypeAVIMovie, (NSString*)kUTTypeVideo, (NSString*)kUTTypeMPEG4];
    //imagePicker.videoQuality = UIImagePickerControllerQualityTypeHigh;
    imagePicker.mediaTypes = @[(NSString*)kUTTypeImage];
    imagePicker.allowsEditing = true;*/
    
    
    UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
    ipc.delegate = __pickerController ;
    if([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
    {
        ipc.sourceType = UIImagePickerControllerSourceTypeCamera;
        ipc.mediaTypes = @[(NSString*)kUTTypeMovie, (NSString*)kUTTypeAVIMovie, (NSString*)kUTTypeVideo, (NSString*)kUTTypeMPEG4];
        ipc.allowsEditing = false;
        
        //ipc.showsCameraControls = NO;
        //ipc.cameraOverlayView =__pickerController.view;
        
        [ipc setVideoMaximumDuration:5];
        [[UIApplication sharedApplication].keyWindow.rootViewController presentViewController:ipc animated:YES completion:NULL];
    }
    else
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"" message:@"No Camera Available." delegate:[UIApplication sharedApplication].keyWindow.rootViewController cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
        [alert show];
        alert = nil;
    }
    
    

    //[[UIApplication sharedApplication].keyWindow.rootViewController presentViewController:imagePicker animated:YES completion:nil];
}


#endif
#endif
