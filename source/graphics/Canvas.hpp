#ifndef _ADJ_CANVAS_H
#define _ADJ_CANVAS_H

#include "opengl/GL2.hpp"
#include "Bitmap.hpp"
#include "Font.hpp"
#include "Color.hpp"
#include "Point.hpp"
#include "Rect.hpp"

#include <deque>

#include "../nanovg/nanovg.h"
struct NVGLUframebuffer;

class View;
using namespace mt;


class Canvas{
private:
	NVGcontext *mNVGContext;
	//std::deque<NVGLUframebuffer*> mNVGFramebufferQueue;
	//NVGLUframebuffer* mNVGFramebufferCurrent;
	int mCanvasWidth;
	int mCanvasHeight;
public:
	NVGcontext * getNVGContext() {
		return mNVGContext;
	}
    
	Canvas(NVGcontext * context);
	~Canvas();


    void begin(int width, int height);
	void end();

    void clear();
    void clear(Color c);

	void translate(Point pos);
	void save();
	void restore();
	void reset();


	void setScissor(int x, int y, int w, int h);
	void resetScissor();

	float calculateFontBounds(Font font, String text, float *bounds);

	void drawBitmap(Bitmap *bitmap, Rect r, double degrees);

	void drawLine(Point p1, Point p2, int width, Color c);
	void drawLineStrip(float *points2d, float *argbColors, int start, int count, int width);
	void drawLineLoop(float *points2d, float *argbColors, int start, int count, int width);
	void drawQuad(Rect r, int borderWithOrfill, Color c);

	void drawQuad(int x, int y, int w, int h, int borderWithOrfill, Color* c);

	void drawPoligon(float *points2d, float *argbColors, int start, int count, int type);

	void drawRoundQuad(Rect r, int cornerRadius, int borderWithOrfill, Color c);


	void drawTriangle(Point pos, int width, int height, Color c);
	void drawFont(Point l, String text, Font font, Color color);


	//void bindVbo(Vbo *vbo, float *vertexs, int vsize, float *colors, int csize);
	//void drawVboLineStrip(Vbo *vbo, int vertexCount, int colorsCount, int start, int count, int width, int type);

	void drawCircle(Point p, int diameter, int borderWithOrfill, Color c);

	void drawTorus(Point p, int diameter, int width, Color c, double fromdegrees = 0, double todegrees = 360, bool drawToLeft = true);

	void drawCube(Point loc, Rect dim, Color c);

	/*
	NVGLUframebuffer* createRenderFrame(int w, int h);
	void bindRenderFrame(NVGLUframebuffer* fb = NULL, int x = 0, int y = 0);
	void drawRenderFrame(int x, int y, NVGLUframebuffer* fb);
	void deleteRenderFrame(NVGLUframebuffer* fb);
	NVGLUframebuffer* getCurrentRenderFrame();
	*/



};


#endif
