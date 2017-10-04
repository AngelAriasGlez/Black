#ifndef _MATHDEFS_H
#define _MATHDEFS_H

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif


inline float clamp(float x, float a, float b)
{
	return x < a ? a : (x > b ? b : x);

}

#endif