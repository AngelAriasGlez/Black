//
//  Header.h
//  ADJ
//
//  Created by Angel Arias Gonzalez on 03/10/13.
//  Copyright (c) 2013 Angel Arias Gonzalez. All rights reserved.
//

#ifndef ADJ_GL_h
#define ADJ_GL_h

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>
    #elif TARGET_OS_MAC
        #include <OpenGL/gl.h>
    #endif
#elif __ANDROID__
	#include <GLES2/gl2.h>
#elif _WIN32
	#include <GL/glew.h>

#endif


#endif
