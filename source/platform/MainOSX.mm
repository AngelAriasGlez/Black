#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_OSX

#import <Foundation/Foundation.h>
#include "Platform.hpp"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
       	int result = 0;
        if (Platform::create()){
            result = Platform::start();
        }
        return result;
    }
    return 0;
}
#endif
#endif
