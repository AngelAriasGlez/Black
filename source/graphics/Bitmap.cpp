

#include "../nanovg/nanovg.h"
#include "../platform/Application.hpp"

Bitmap::Bitmap():Id(0){
	Data = nullptr;
	Size = 0;
}
Bitmap::~Bitmap() {
	auto ctx = Application::getInstance()->getNVGContext();
	nvgDeleteImage(ctx, Id);
}
void Bitmap::load(String path) {
	auto ctx = Application::getInstance()->getNVGContext();
	if (Id) {
		nvgDeleteImage(ctx, Id);
	}
	Id = nvgCreateImage(ctx, path.c_str(), 0);
	nvgImageSize(ctx, Id, &Width, &Height);
}
void Bitmap::load(unsigned char *data, int size) {
	auto ctx = Application::getInstance()->getNVGContext();
	if (Id) {
		nvgDeleteImage(ctx, Id);
	}
	Id = nvgCreateImageMem(ctx, 0, data, size);
	nvgImageSize(ctx, Id, &Width, &Height);

}

void Bitmap::loadRawToRGBA(unsigned char* dt, int sz) {
	int n;
	Data = stbi_load_from_memory(dt, sz, &Width, &Height, &n, 4);
	Size = sz;
}
void Bitmap::createTexture() {
	Id = nvgCreateImageRGBA(Application::getInstance()->getNVGContext(), Width, Height, 0, Data);
}

