#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>


#define  LOG_TAG    "DFW"

/*
	ANDROID_LOG_UNKNOWN = 0,
    ANDROID_LOG_DEFAULT,
    ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG,
    ANDROID_LOG_INFO,
    ANDROID_LOG_WARN,
    ANDROID_LOG_ERROR,
    ANDROID_LOG_FATAL,
    ANDROID_LOG_SILENT,
 */

#ifdef __ANDROID__
	#include <android/log.h>
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
	#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
	#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#else
	#define LOGI(...) printf(__VA_ARGS__);  \
	printf("\n")
	#define LOGE(...) printf("E: "); \
	printf(__VA_ARGS__); \
	printf("\n")
	#define LOGW(...) printf("W: "); \
	printf(__VA_ARGS__); \
	printf("\n")

	#define LOGX() std::cout
	#define ENDL() std::endl
#endif

#endif // LOGGER_H
