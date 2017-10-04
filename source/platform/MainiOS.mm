
#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE


#import <Foundation/Foundation.h>

#include "Platform.hpp"

/**
 * Main entry point.
 */
int main(int argc, char** argv)
{

	int result = 0;
	if (Platform::create()){
		result = Platform::start();
	}
    return result;
}

#endif
#endif
