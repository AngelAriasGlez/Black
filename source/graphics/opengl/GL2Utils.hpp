#ifndef GL2UTILS_H
#define GL2UTILS_H

#include <iostream>
#include <fstream>
#include <string>



#include "GL2.hpp"

//#include <GLES2/gl2platform.h>


#include "../../utils/Log.hpp"

inline static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

inline static void GL(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

inline GLuint glxLoadShaderFromFile(const char* filename, GLenum shaderType) {

	std::ifstream file(filename);
	if (!file) {
		LOGE("Unable to open file: %s", filename);
		return 0;
	}

	char line[256];
	std::string source;

	while (file) {
		file.getline(line, 256);
		source += line;
		source += '\n';
	}

    if (!file.eof()) {
    	LOGE("Unable to open file: %s", filename);
    	return 0;
    }

    GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const char**)&source, NULL);
	glCompileShader(shader);
	GL("glCompileShader()");

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen) {
            char* buf = (char*) malloc(infoLen);
            if (buf) {
                glGetShaderInfoLog(shader, infoLen, NULL, buf);
                LOGE("Could not compile shader %d:\n%s\n",
                        shaderType, buf);
                free(buf);
            }
            glDeleteShader(shader);
            shader = 0;
        }
    }

	return shader;
}


inline GLuint glxLoadShader(const char* pSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    GL("glCreateShader()");
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        GL("glShaderSource()");
        glCompileShader(shader);
        GL("glShaderSource()");
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        GL("glGetShaderiv()");
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            GL("glGetShaderiv()");
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }else{
    	LOGE("Could not create shader");
    }

    return shader;
}



inline GLuint glxCreateProgram(const char* pVertex, const char* pFragment) {
    GLuint vertexShader = glxLoadShader(pVertex, GL_VERTEX_SHADER);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = glxLoadShader(pFragment, GL_FRAGMENT_SHADER);
    if (!pixelShader) {
        return 0;
    }


    GLuint program = glCreateProgram();
    GL("glCreateProgram()");
    if (program) {
        glAttachShader(program, vertexShader);
        GL("glAttachShader");
        glAttachShader(program, pixelShader);
        GL("glAttachShader");
        glLinkProgram(program);
        GL("glAttachShader()");
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    LOGI("program %d", program);
    return program;
}
inline void glGenTextureFromFramebuffer(GLuint *t, GLuint *f, GLuint *d, GLsizei w, GLsizei h){

		glGenTextures(1, t);
		glBindTexture(GL_TEXTURE_2D, *t);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   
		glGenRenderbuffers(1, d);
		glBindRenderbuffer(GL_RENDERBUFFER, *d);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);

		glGenFramebuffers(1, f);
		glBindFramebuffer(GL_FRAMEBUFFER, *f);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *t, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *d);
    
    /*glGenRenderbuffers(1, f);
    glBindRenderbuffer(GL_RENDERBUFFER, *f);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, *f);*/

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE){
			LOGE("Framebuffer status: %d", (int)status);
		}
    
    /*glGenTextures(1, t);
    glBindTexture(GL_TEXTURE_2D, *t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *t, 0);*/
    
    glBindTexture(GL_TEXTURE_2D, 0);

		//glBindFramebuffer( GL_FRAMEBUFFER, 0 ); // 0 not work in iOS
		//glBindRenderbuffer( GL_RENDERBUFFER, 0 );

}

#endif
