#ifndef _ADJ_VIDEOVIEW_H
#define _ADJ_VIDEOVIEW_H

#include "video/CvVideoSource.hpp"
#include "video/VideoSource.hpp"
#include <chrono>
#include "../View.hpp"
#include "../../graphics/Canvas.hpp"

using namespace std::chrono;

class VideoView : public View{
protected:
	VideoSource *mSource;

	int imgid = 0;
	milliseconds starttime;
	int framecount;

	int imgW;
	int imgH;
	
    mt::Point offset;

	std::vector<int> mThumbs;
public:

	VideoView(): View(){
		initialize();
	}
	VideoView(Rect r) : View(r){
		initialize();
	}
	~VideoView() {

	}


	virtual std::string toString() override{
		return "VideoView::" + View::toString();
	}
	virtual View *clone() const override { return new VideoView(*this); };

	void setSource(VideoSource *source) {
		mSource = source;

	}

	void initialize(){

	}
	

    virtual void draw(Canvas* canvas) override{
		View::draw(canvas);
        if(!mSource) return;
		auto msframe = 1000.0 / (double)mSource->getFramerate();

		if (!starttime.count()) starttime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		auto rem = (starttime.count() + ((double)framecount*msframe)) - now.count();
		if (rem < 0) {
			starttime = now;
			framecount = 0;
		}

		auto vg = canvas->getNVGContext();

		if(rem <= 0){
			int dw = getWidth();
			int dh = getHeight();


			int w = mSource->getWidth();
			int h = mSource->getHeight();

			auto ptr = mSource->read();
			if (ptr) {
				if (imgid <= 0) {
					imgid = nvgCreateImageRGB(vg, w, h, 0, ptr);
				}
				else {
					if (imgH != h || imgW != w) {
						nvgDeleteImage(vg, imgid);
						imgid = nvgCreateImageRGB(vg, w, h, 0, ptr);
						imgH = h;
						imgW = w;

					}
					else {
						nvgUpdateImage(vg, imgid, ptr);
					}
				}
				framecount++;
			}

		}

		if (imgid > 0) {

			Rect is(getVideoSurfaceX(), getVideoSurfaceY(), getVideoSurfaceWidth(), getVideoSurfaceHeight());

			nvgBeginPath(vg);

			NVGpaint imgPaint;
			int trotation = mSource->getRotation();
			if (trotation == 90) {
				imgPaint = nvgImagePattern(vg, is.getX() + getVideoSurfaceWidth(), is.getY(), is.getHeight(), is.getWidth(), nvgDegToRad(trotation), imgid, 1.0f);
			}else if (trotation == 270) {
				imgPaint = nvgImagePattern(vg, is.getX(), is.getY()+ getVideoSurfaceHeight(), is.getHeight(), is.getWidth(), nvgDegToRad(trotation), imgid, 1.0f);
			}else if (trotation == 180) {
				imgPaint = nvgImagePattern(vg, is.getX() + getVideoSurfaceWidth(), is.getY() + getVideoSurfaceHeight(), is.getWidth(), is.getHeight(), nvgDegToRad(trotation), imgid, 1.0f);
			}else {
				imgPaint = nvgImagePattern(vg, is.getX(), is.getY(), is.getWidth(), is.getHeight(), 0, imgid, 1.0f);
			}
			nvgRect(vg, is.getX(), is.getY(), is.getWidth(), is.getHeight());
			nvgFillPaint(vg, imgPaint);
			nvgFill(vg);

		}



	}




	int getVideoSurfaceHeight() {
		return (getVideoSurfaceWidth() / mSource->getAspectRatio());

	}
	int getVideoSurfaceWidth() {
		auto h = getWidth() / mSource->getAspectRatio();
		if (h > getHeight()) {
			return (double)((double)getWidth() / (h / (double)getHeight())) * (mZoom);
		}else {
			return (double)getWidth() * mZoom;
		}
	}
	int getVideoSurfaceY() {
		return ((getHeight() - getVideoSurfaceHeight()) / 2) + offset.y;

	}
	int getVideoSurfaceX() {
		return ((getWidth() - getVideoSurfaceWidth()) / 2) + offset.x;
	}

    float pinchStartZoom;
	bool onTouchEvent(TouchEvent e) override {
		if (e.type == TouchEvent::WHEEL) {
			setZoom(mZoom + e.data);
		}else if (e.type == TouchEvent::PINCH_START) {
            pinchStartZoom = mZoom;
        }else if (e.type == TouchEvent::PINCH) {
            setZoom(pinchStartZoom * e.data);
        }else if (e.type == TouchEvent::PAN) {
            updateOffset(mt::Point(e.dragDiff.x+offset.x,e.dragDiff.y+offset.y));

        }
        
		
		return true;
	}
    
    void updateOffset(mt::Point diff){
        
        int xmax = Utils::max((getVideoSurfaceWidth()-getWidth()) / 2, 0);
        int x = Utils::clamp(diff.x, -xmax, xmax);
        
        int ymax = Utils::max((getVideoSurfaceHeight()-getHeight()) / 2, 0);

        int y = Utils::clamp(diff.y, -ymax, ymax);
        offset = mt::Point(x, y);
    }


	double mZoom = 1;
	double getZoom() {
		return mZoom;
	}
	void setZoom(double zoom) {
		mZoom = Utils::clamp(zoom, 1.0, 5.0);
        updateOffset(offset);
	}

	virtual void update(Canvas *canvas)  override {


	}
};

#endif
