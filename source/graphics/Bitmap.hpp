#ifndef _ADJ_BITMAP_H
#define _ADJ_BITMAP_H

#include "../core/String.hpp"
#include "../nanovg/stb_image.h"

class Bitmap{
public:
	int Id;
	int Width;
	int Height;

	Bitmap();
	~Bitmap();
	void load(String path);
	void load(unsigned char *data, int size);

	unsigned char *Data;
	int Size;

	/*int getWidth(){
		return mbDimension.width;
	}
	int getHeight(){
		return mbDimension.height;
	}

    int getChannels(){
		return mChannels;
	}
	int getSize() {
		return mbDimension.width * mbDimension.height * mChannels;
	}*/
	void loadRawToRGBA(unsigned char* dt, int sz);
	void createTexture();


	inline unsigned char getpixel(unsigned y, unsigned x, int channel)
	{
		const int channels = 4;
		if (x < Width && y < Height)
			return Data[(y * channels * (int)Width) + (channels * x) + channel];

		return 0;
	}
	inline unsigned char saturate(float x)
	{
		return x > 255.0f ? 255
			: x < 0.0f ? 0
			: (unsigned char)x;
	}

	void resize(int newWidth, int newHeight){
		const int channels = 4;
		unsigned char *out = new unsigned char[newWidth * newHeight * channels];

		const float tx = Width / newWidth;
		const float ty = Height / newHeight;
		const std::size_t row_stride = newWidth * channels;

		unsigned char C[5] = { 0 };

		for (int i = 0; i < newHeight; ++i)
		{
			for (int j = 0; j < newWidth; ++j)
			{
				const float x = float(tx * j);
				const float y = float(ty * i);
				const float dx = tx * j - x;
				const float dy = ty * i - y;

				for (int k = 0; k < 4; ++k)
				{
					for (int jj = 0; jj < 4; ++jj)
					{
						const int z = y - 1 + jj;
						float a0 = getpixel(z, x, k);
						float d0 = getpixel(z, x - 1, k) - a0;
						float d2 = getpixel(z, x + 1, k) - a0;
						float d3 = getpixel(z, x + 2, k) - a0;
						float a1 = -1.0 / 3 * d0 + d2 - 1.0 / 6 * d3;
						float a2 = 1.0 / 2 * d0 + 1.0 / 2 * d2;
						float a3 = -1.0 / 6 * d0 - 1.0 / 2 * d2 + 1.0 / 6 * d3;
						C[jj] = a0 + a1 * dx + a2 * dx * dx + a3 * dx * dx * dx;

						d0 = C[0] - C[1];
						d2 = C[2] - C[1];
						d3 = C[3] - C[1];
						a0 = C[1];
						a1 = -1.0 / 3 * d0 + d2 - 1.0 / 6 * d3;
						a2 = 1.0 / 2 * d0 + 1.0 / 2 * d2;
						a3 = -1.0 / 6 * d0 - 1.0 / 2 * d2 + 1.0 / 6 * d3;
						out[i * row_stride + j * channels + k] = saturate(a0 + a1 * dy + a2 * dy * dy + a3 * dy * dy * dy);
					}
				}
			}
		}
		delete[] Data;

		Data = out;

		Width = newWidth;
		Height = newHeight;
	}



};


#endif
