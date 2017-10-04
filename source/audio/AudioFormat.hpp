
#ifndef _ADJ_ADJ_H
#define _ADJ_ADJ_H




#define SAMPLE_INT16 33
#define SAMPLE_FLOAT32 44


#include <limits.h>

#ifndef SOUTPUT_FORMAT
	#define SOUTPUT_FORMAT SAMPLE_FLOAT32
#endif

#if SOUTPUT_FORMAT == SAMPLE_INT16
typedef short Sample;
#define SAMPLE_MAX SHRT_MAX
#define SAMPLE_MIN SHRT_MIN
#elif SOUTPUT_FORMAT == SAMPLE_FLOAT32
typedef float Sample;
#define SAMPLE_MAX 1.0f
#define SAMPLE_MIN -1.0f
#endif




static float SampleToFloat32(Sample sample){
	if (SOUTPUT_FORMAT == SAMPLE_INT16){
		return (float)sample / (float)SHRT_MAX;
	}
	return sample;
}

static signed short SampleToInt16(Sample sample){
	if (SOUTPUT_FORMAT == SAMPLE_FLOAT32){
		return (signed short)((float)sample*(float)SHRT_MAX);
	}
	return  (signed short)sample;
}


static inline Sample sclamp(double s){
	if (SAMPLE_MAX < s)
		return SAMPLE_MAX;
	else if (SAMPLE_MIN > s)
		return SAMPLE_MIN;
	else
		return ((Sample)s);

}


#endif