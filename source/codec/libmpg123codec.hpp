#ifndef _ADJ_LIBMPG123CODEC_H
#define _ADJ_LIBMPG123CODEC_H

#include "Codec.hpp"

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <mpg123.h>


#include <iostream>

class Libmpg123Codec : public Codec {
public:


	Libmpg123Codec();
	~Libmpg123Codec();

	bool _open();
	int _read(void* pointer, int frameCount);
	void _seek(int frameNumber);

private:
	mpg123_handle *mHandle;
	int mResult;

};


#endif


