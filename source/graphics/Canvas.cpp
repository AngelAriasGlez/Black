#include "Canvas.hpp"
#include "Platform.hpp"


Canvas::Canvas(NVGcontext* context) {
	//mNVGFramebufferCurrent = nullptr;
	mNVGContext = context;
}

void Canvas::begin(int width, int height){
	mCanvasWidth = width;
	mCanvasHeight = height;
	glViewport(0, 0, mCanvasWidth, mCanvasHeight);
    nvgBeginFrame(mNVGContext, mCanvasWidth, mCanvasHeight, Platform::getDisplayScale());
}
void Canvas::end() {
	nvgEndFrame(mNVGContext);
}

Canvas::~Canvas(){

}
void Canvas::clear(){
	clear(Color(0, 0, 0, 0));
}
void Canvas::clear(Color c){
	glClearColor(c.fr(), c.fg(), c.fb(), c.fa());
	glClear(GL_COLOR_BUFFER_BIT);
}
void Canvas::drawQuad(Rect r, int borderWithOrfill, Color color) {
	nvgBeginPath(mNVGContext);
	nvgRect(mNVGContext, r.x, r.y, r.getWidth(), r.getHeight());
	nvgFillColor(mNVGContext, color);
	nvgFill(mNVGContext);
}
void Canvas::drawQuad(int x, int y, int w, int h, int borderWithOrfill, Color* c) {
	nvgBeginPath(mNVGContext);
	nvgRect(mNVGContext, x, y, w, h);
	nvgFillColor(mNVGContext, *c);
	nvgFill(mNVGContext);
}

void Canvas::drawFont(Point l, String text, Font font, Color color) {
	nvgTextAlign(mNVGContext, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFillColor(mNVGContext, color);
	nvgFontSize(mNVGContext, font.getSize());
	nvgFontFace(mNVGContext, font.getName());
	nvgText(mNVGContext, (int)l.x, (int)l.y, text, nullptr);
}
float Canvas::calculateFontBounds(Font font, String text, float *bounds) {
	nvgTextAlign(mNVGContext, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFontSize(mNVGContext, font.getSize());
	nvgFontFace(mNVGContext, font.getName());
	return nvgTextBounds(mNVGContext, 0, 0, text, nullptr, bounds);

}

void Canvas::translate(Point pos) {
	nvgTranslate(mNVGContext, pos.x, pos.y);
}
void Canvas::save() {
	nvgSave(mNVGContext);
}
void Canvas::restore() {
	nvgRestore(mNVGContext);
}
void Canvas::reset() {
	nvgReset(mNVGContext);
}
void Canvas::setScissor(int x, int y, int w, int h) {
	nvgIntersectScissor(mNVGContext, x, y, w, h);
}
void Canvas::resetScissor() {
	nvgResetScissor(mNVGContext);
}

void Canvas::drawBitmap(Bitmap *bm, Rect r, double degrees){
	auto imgPaint = nvgImagePattern(mNVGContext, 0, 0, r.getWidth(), r.getHeight(), 0, bm->Id, 1.0);
	nvgBeginPath(mNVGContext);
	nvgRect(mNVGContext, r.x, r.y, r.getWidth(), r.getHeight());
	nvgFillPaint(mNVGContext, imgPaint);
	nvgFill(mNVGContext);

}


void Canvas::drawRoundQuad(Rect r, int cornerRadius, int borderWithOrfill, Color c) {
	nvgBeginPath(mNVGContext);
	nvgRoundedRect(mNVGContext, r.x, r.y, r.getWidth(), r.getHeight(), cornerRadius);
	nvgFillColor(mNVGContext, c);
	nvgFill(mNVGContext);



}
void Canvas::drawLine(Point p1, Point p2, int width, Color c){

}

void Canvas::drawTriangle(Point pos, int width, int height, Color c){}
void Canvas::drawCircle(Point p, int diameter, int borderWithOrfill, Color c){
	nvgBeginPath(mNVGContext);
	nvgCircle(mNVGContext, p.x, p.y, diameter/2);
	nvgFillColor(mNVGContext, c);
	nvgFill(mNVGContext);

}
void Canvas::drawTorus(Point p, int diameter, int width, Color c, double fromdegrees, double todegrees, bool drawToLeft){}
void Canvas::drawCube(Point l, Rect d, Color c) {}
void Canvas::drawLineStrip(float *points2d, float *argbColors, int start, int count, int width) {}
void Canvas::drawLineLoop(float *points2d, float *argbColors, int start, int count, int width) {}
void Canvas::drawPoligon(float *points2d, float *argbColors, int start, int count, int type) {}






/*

NVGLUframebuffer* Canvas::createRenderFrame(int w, int h) {
return nvgluCreateFramebuffer(mNVGContext, w, h, 0);
}
void Canvas::bindRenderFrame(NVGLUframebuffer* fb, int x, int y) {
end();

if (fb != NULL) {
int fboWidth, fboHeight;
nvgImageSize(mNVGContext, fb->image, &fboWidth, &fboHeight);
nvgluBindFramebuffer(fb);
mNVGFramebufferCurrent = fb;
glViewport(-x, y, fboWidth, fboHeight);
nvgBeginFrame(mNVGContext, fboWidth, fboHeight, 1);
}else {
nvgluBindFramebuffer(NULL);
mNVGFramebufferCurrent = NULL;
glViewport(0, 0, mCanvasWidth, mCanvasHeight);
nvgBeginFrame(mNVGContext, mCanvasWidth, mCanvasHeight, 1);
}
}

void Canvas::drawRenderFrame(int x, int y, NVGLUframebuffer* fb) {
int fboWidth, fboHeight;
nvgImageSize(mNVGContext, fb->image, &fboWidth, &fboHeight);
NVGpaint fbPaint = nvgImagePattern(mNVGContext, x, y, fboWidth, fboHeight, 0, fb->image, 1.0f);
nvgBeginPath(mNVGContext);
nvgRect(mNVGContext, x, y, fboWidth, fboHeight);
nvgFillPaint(mNVGContext, fbPaint);
nvgFill(mNVGContext);

}
void Canvas::deleteRenderFrame(NVGLUframebuffer* fb) {
nvgluDeleteFramebuffer(fb);
}
NVGLUframebuffer* Canvas::getCurrentRenderFrame() {
return mNVGFramebufferCurrent;
}*/
